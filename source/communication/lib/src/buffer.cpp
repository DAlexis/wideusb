#include "buffer.hpp"
#include "ring-buffer.h"

#include <cstring>

std::shared_ptr<Buffer> Buffer::create(size_t size, const uint8_t* init_data )
{
    return std::shared_ptr<Buffer>(new Buffer(size, init_data));
}

PBuffer Buffer::create(RingBuffer& data, size_t size)
{
    PBuffer result = create();
    result->append(data,  size);
    return result;
}

PBuffer Buffer::create(RingBuffer& data)
{
    return create(data, ring_buffer_data_size(&data));
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

const Buffer& Buffer::operator>>(RingBuffer& target) const
{
    ring_buffer_put_data(&target, data(), size());
    return *this;
}

Buffer& Buffer::operator<<(const Buffer& buf)
{
    return append(buf.data(), buf.size());
}

Buffer& Buffer::operator<<(Buffer&& buf)
{
    *this << buf;
    buf.clear();
    return *this;
}

Buffer& Buffer::operator<<(RingBuffer& ring_buffer)
{
    append(ring_buffer, ring_buffer_data_size(&ring_buffer));
    return *this;
}

std::vector<uint8_t>& Buffer::contents()
{
    return m_contents;
}

Buffer& Buffer::append(const uint8_t* data, size_t size)
{
    size_t old_size = m_contents.size();
    m_contents.resize(old_size + size);
    memcpy(m_contents.data() +  old_size, data, size);
    return *this;
}

Buffer& Buffer::append(RingBuffer& data, size_t size)
{
    size_t buffer_size = ring_buffer_data_size(&data);
    if (buffer_size < size)
        size  = buffer_size;
    size_t old_size = m_contents.size();
    m_contents.resize(old_size + size);
    ring_buffer_get_data(&data, m_contents.data() +  old_size, size);
    return *this;
}
