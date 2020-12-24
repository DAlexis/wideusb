#include "buffer.hpp"

#include <cstring>
#include <string>

SerialWriteAccessor& SerialWriteAccessor::operator<<(SerialReadAccessor&& accessor)
{
    return *this << accessor;
}

SerialWriteAccessor& SerialWriteAccessor::operator<<(SerialReadAccessor& accessor)
{
    put(accessor, accessor.size());
    return *this;
}

std::shared_ptr<Buffer> Buffer::create(size_t size, const void* init_data)
{
    return std::shared_ptr<Buffer>(new Buffer(size, init_data));
}

PBuffer Buffer::create(RingBuffer& data, size_t size)
{
    PBuffer result = create();
    result->put(data, size);
    return result;
}

PBuffer Buffer::create(RingBuffer& data)
{
    return create(data, data.size());
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

void Buffer::extend(size_t size)
{
    m_contents.resize(m_contents.size() + size);
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

uint8_t& Buffer::operator[](size_t pos)
{
    return m_contents[pos];
}

bool Buffer::operator==(const Buffer& right) const
{
    return m_contents == right.m_contents;
}

std::vector<uint8_t>& Buffer::contents()
{
    return m_contents;
}

void Buffer::put(const void* data, size_t size)
{
    size_t old_size = m_contents.size();
    extend(size);

    memcpy(m_contents.data() + old_size, data, size);
}

void Buffer::put(SerialReadAccessor& accessor, size_t size)
{
    if (size == 0)
        return;

    size_t buffer_size = accessor.size();
    if (buffer_size < size)
        size = buffer_size;

    size_t old_size = m_contents.size();
    extend(size);
    accessor.get(m_contents.data() + old_size, size);
}

BufferAccessor::BufferAccessor(PBuffer buf, size_t pos) :
    m_buffer(buf),
    m_offset(pos)
{
}

void BufferAccessor::skip(size_t count)
{
    m_offset += count;
}

void BufferAccessor::get(uint8_t* buf, size_t size)
{
    memcpy(buf, m_buffer->data() + m_offset, size);
    m_offset += size;
}

size_t BufferAccessor::size() const
{
    return m_buffer->size() - m_offset;
}

uint8_t BufferAccessor::operator[](size_t pos) const
{
    return (*m_buffer)[pos + m_offset];
}

RingBuffer::RingBuffer(size_t capacity) :
    m_contents(capacity + 1)
{
}

size_t RingBuffer::free_space()
{
    if (m_p_read <= m_p_write)
    {
        return m_contents.size() + m_p_read - m_p_write - 1;
    } else {
        return m_p_read - m_p_write - 1;
    }
}

size_t RingBuffer::size() const
{
    if (m_p_read <= m_p_write)
    {
        return m_p_write - m_p_read;
    } else {
        return m_contents.size() + m_p_write - m_p_read;
    }
}

void RingBuffer::put(const void* src, size_t size)
{
    const uint8_t* buf = (const uint8_t*) src;
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

void RingBuffer::put(SerialReadAccessor& accessor, size_t size)
{
    size_t free_tail = m_contents.size() - m_p_write;
    if (size < free_tail)
    {
        // Add to the end
        accessor.get(&m_contents[m_p_write], size);
        m_p_write += size;
    } else {
        // Part add to the end and part add to the beginning
        accessor.get(&m_contents[m_p_write], free_tail);
        uint32_t second_part_size = size - free_tail;
        accessor.get(&m_contents[0], second_part_size);
        m_p_write = second_part_size;
    }
}

void RingBuffer::get(uint8_t* buf, size_t size)
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

void RingBuffer::skip(size_t size)
{
    m_p_read += size;
    if (m_p_read >= m_contents.size())
        m_p_read -= m_contents.size();
}

bool RingBuffer::empty() const
{
    return m_p_read == m_p_write;
}

void RingBuffer::clear()
{
    m_p_write = m_p_read = 0;
}

uint8_t RingBuffer::operator[](size_t pos) const
{
    return const_cast<RingBuffer*>(this)->operator[](pos);
}

uint8_t& RingBuffer::operator[](size_t pos)
{
    size_t target_pos = pos + m_p_read;
    if (target_pos >= m_contents.size())
    {
        target_pos -= m_contents.size();
    }
    return m_contents[target_pos];
}

RingBufferHandle RingBuffer::handle()
{
    RingBufferHandle h;
    h.p_ring_buffer = this;
    return h;
}
