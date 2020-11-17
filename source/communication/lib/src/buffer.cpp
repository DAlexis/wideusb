#include "buffer.hpp"
#include <cstring>

std::shared_ptr<Buffer> Buffer::create(size_t size, const uint8_t* init_data )
{
    return std::shared_ptr<Buffer>(new Buffer(size, init_data));
}

Buffer::Buffer(size_t size, const uint8_t* init_data) :
    m_contents(size)
{
    if (init_data != nullptr)
    {
        memcpy(m_contents.data(), init_data, size);
    } else {
        memset(m_contents.data(), 0, size);
    }
}

PBuffer Buffer::clone()
{
    PBuffer copy = create(size(), data());
    return copy;
}

size_t Buffer::size() const
{
    return m_contents.size();
}

uint8_t* Buffer::data()
{
    return m_contents.data();
}

const  uint8_t* Buffer::data() const
{
    return m_contents.data();
}

void Buffer::clear()
{
    m_contents.clear();
}

Buffer& Buffer::operator<<(const Buffer& buf)
{
    size_t old_size = m_contents.size();
    m_contents.resize(old_size + buf.size());
    memcpy(m_contents.data() +  old_size, buf.data(), buf.size());
    return *this;
}

Buffer& Buffer::operator<<(Buffer&& buf)
{
    *this << buf;
    buf.clear();
    return *this;
}

std::vector<uint8_t>& Buffer::contents()
{
    return m_contents;
}
