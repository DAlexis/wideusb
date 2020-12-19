#include "buffer.hpp"
#include "ring-buffer.h"

#include <cstring>

std::shared_ptr<Buffer> Buffer::create(size_t size, const void* init_data )
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

Buffer::Buffer(size_t size, const void* init_data) :
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

bool Buffer::operator==(const Buffer& right) const
{
    return m_contents == right.m_contents;
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
    if (size == 0)
        return *this;

    size_t buffer_size = ring_buffer_data_size(&data);
    if (buffer_size < size)
        size  = buffer_size;
    size_t old_size = m_contents.size();
    m_contents.resize(old_size + size);
    ring_buffer_get_data(&data, m_contents.data() +  old_size, size);
    return *this;
}

RingBufferClass::RingBufferClass(size_t capacity) :
    m_contents(capacity + 1)
{
}

size_t RingBufferClass::free_space()
{
    if (m_p_read <= m_p_write)
    {
        return m_contents.size() + m_p_read - m_p_write - 1;
    } else {
        return m_p_read - m_p_write - 1;
    }
}

size_t RingBufferClass::data_size()
{
    if (m_p_read <= m_p_write)
    {
        return m_p_write - m_p_read;
    } else {
        return m_contents.size() + m_p_write - m_p_read;
    }
}

void RingBufferClass::put(const uint8_t* buf, size_t size)
{
    size_t free_tail = m_contents.size() - m_p_write;
    if (size < free_tail)
    {
        // Add to the end
        memcpy(&m_contents[m_p_write], buf, size);
        m_p_write += size;
    } else {
        // Part add to the end and part add to the beginning
        memcpy(&m_contents[m_p_write], buf, free_tail);
        uint32_t second_part_size = size - free_tail;
        memcpy(&m_contents[0], &buf[free_tail], second_part_size);
        m_p_write = second_part_size;
    }
}

void RingBufferClass::get(uint8_t* buf, size_t size)
{
    if (m_p_write >= m_p_read)
    {
        memcpy(buf, &m_contents[m_p_read], size);
        m_p_read += size;
    } else {
        uint32_t tail = m_contents.size() - m_p_read;
        if (tail > size)
        {
            memcpy(buf, &m_contents[m_p_read], size);
            m_p_read += size;
        } else {
            memcpy(buf, &m_contents[m_p_read], tail);
            memcpy(buf+tail, &m_contents[0], size - tail);
            m_p_read = size - tail;
        }
    }
}

void RingBufferClass::skip(size_t size)
{
    m_p_read += size;
    if (m_p_read >= m_contents.size())
        m_p_read -= m_contents.size();
}

uint8_t* RingBufferClass::operator[](size_t pos)
{
    size_t target_pos = pos + m_p_read;
    if (target_pos > m_contents.size())
    {
        target_pos -= m_contents.size();
    }
    return &m_contents[target_pos];
}
