#include "wideusb/buffer.hpp"

#include <cstring>
#include <string>

bool SerialReadAccessor::empty() const
{
    return size() == 0;
}

void SerialReadAccessor::extract(uint8_t* buf, size_t size)
{
    get(buf, size);
    skip(size);
}

PBuffer SerialReadAccessor::extract_buf(size_t extraction_size)
{
    extraction_size = std::min(extraction_size, size());
    PBuffer result = Buffer::create(extraction_size);
    extract(result->data(), result->size());
    return result;
}

PBuffer SerialReadAccessor::extract_buf()
{
    PBuffer result = Buffer::create(size());
    extract(result->data(), result->size());
    return result;
}

bool SerialWriteAccessor::put(SerialReadAccessor& buffer)
{
    return put(buffer, buffer.size());
}

bool SerialWriteAccessor::put(SerialReadAccessor&& buffer)
{
    return put(buffer);
}

SerialWriteAccessor& SerialWriteAccessor::operator<<(SerialReadAccessor&& accessor)
{
    return *this << accessor;
}

SerialWriteAccessor& SerialWriteAccessor::operator<<(SerialReadAccessor& accessor)
{
    put(accessor, accessor.size());
    return *this;
}

SerialWriteAccessor::RawStream SerialWriteAccessor::raw()
{
    return RawStream(*this);
}

std::shared_ptr<Buffer> Buffer::create(size_t size, const void* init_data)
{
    return std::shared_ptr<Buffer>(new Buffer(size, init_data));
}

PBuffer Buffer::create(SerialReadAccessor& data, size_t size)
{
    PBuffer result = create();
    result->put(data, size);
    return result;
}

PBuffer Buffer::create(SerialReadAccessor& data)
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

PBuffer Buffer::clone() const
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

const uint8_t* Buffer::data() const
{
    return m_contents.data();
}

void Buffer::clear()
{
    m_contents.clear();
}

uint8_t& Buffer::at(size_t pos)
{
    return m_contents[pos];
}

uint8_t& Buffer::operator[](size_t pos)
{
    return at(pos);
}

bool Buffer::operator==(const Buffer& right) const
{
    return m_contents == right.m_contents;
}

std::vector<uint8_t>& Buffer::contents()
{
    return m_contents;
}

bool Buffer::put(const void* data, size_t size)
{
    if (size == 0)
        return true;

    size_t old_size = m_contents.size();
    extend(size);

    memcpy(m_contents.data() + old_size, data, size);
    return true;
}

bool Buffer::put(SerialReadAccessor& accessor, size_t size)
{
    if (size == 0)
        return true;

    size_t buffer_size = accessor.size();
    if (buffer_size < size)
        size = buffer_size;

    size_t old_size = m_contents.size();
    extend(size);
    accessor.extract(m_contents.data() + old_size, size);
    return true;
}

bool Buffer::will_fit(size_t)
{
    return true;
}

BufferAccessor::BufferAccessor(PBuffer buf, size_t pos, size_t limit) :
    m_buffer(buf),
    m_offset(pos),
    m_limit(limit)
{
}

BufferAccessor::BufferAccessor(const BufferAccessor& buf, size_t pos, size_t limit) :
    m_buffer(buf.m_buffer),
    m_offset(buf.m_offset + pos),
    m_limit(limit == std::numeric_limits<size_t>::max()
            ? std::min(m_buffer->size(), buf.m_limit)
            : std::min(m_buffer->size(), std::min(buf.m_offset + limit, buf.m_limit)))
{
}

void BufferAccessor::skip(size_t count)
{
    m_offset += count;
}

void BufferAccessor::get(uint8_t* buf, size_t size) const
{
    memcpy(buf, m_buffer->data() + m_offset, size);
}

size_t BufferAccessor::size() const
{
    return std::min(m_buffer->size(), m_limit) - m_offset;
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

bool RingBuffer::put(const void* src, size_t size)
{
    if (!will_fit(size))
        return false;

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
    return true;
}

bool RingBuffer::put(SerialReadAccessor& accessor, size_t size)
{
    if (!will_fit(size))
        return false;

    size_t free_tail = m_contents.size() - m_p_write;
    if (size < free_tail)
    {
        // Add to the end
        accessor.extract(&m_contents[m_p_write], size);
        m_p_write += size;
    } else {
        // Part add to the end and part add to the beginning
        accessor.extract(&m_contents[m_p_write], free_tail);
        uint32_t second_part_size = size - free_tail;
        accessor.extract(&m_contents[0], second_part_size);
        m_p_write = second_part_size;
    }

    return true;
}

bool RingBuffer::will_fit(size_t size)
{
    return free_space() >= size;
}

void RingBuffer::get(uint8_t* buf, size_t size) const
{
    uint32_t p_read = m_p_read;
    if (m_p_write >= p_read)
    {
        memcpy(buf, &m_contents[p_read], size);
        p_read += size;
    } else {
        uint32_t tail = m_contents.size() - p_read;
        if (tail > size)
        {
            memcpy(buf, &m_contents[p_read], size);
            p_read += size;
        } else {
            memcpy(buf, &m_contents[p_read], tail);
            memcpy(buf+tail, &m_contents[0], size - tail);
        }
    }
}

void RingBuffer::extract(uint8_t* buf, size_t size)
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

MemBlock RingBuffer::get_continious_block(size_t size) const
{
    MemBlock result;
    result.data = &m_contents[m_p_read];
    if (m_p_read <= m_p_write)
    {
        result.size = std::min(uint32_t(size), m_p_write - m_p_read);
    } else {
        result.size = std::min(uint32_t(size), uint32_t(m_contents.size()) - m_p_read);
    }
    return result;
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

SegmentBuffer::SegmentBuffer(PBuffer buf)
{
    if (buf)
        push_back(buf);
}

void SegmentBuffer::push_front(PBuffer buf)
{
    m_segments.push_front(buf);
}

void SegmentBuffer::push_back(PBuffer buf)
{
    m_segments.push_back(buf);
}

void SegmentBuffer::push_front(SegmentBuffer& buf)
{
    for (auto it = buf.m_segments.rbegin(); it != buf.m_segments.rend(); --it)
    {
        push_front(*it);
    }
}

void SegmentBuffer::push_back(SegmentBuffer& buf)
{
    for (auto it = buf.m_segments.begin(); it != buf.m_segments.end(); ++it)
    {
        push_back(*it);
    }
}

PBuffer SegmentBuffer::merge()
{
    for (auto it = std::next(m_segments.begin()); it != m_segments.end(); it = m_segments.erase(it))
    {
        m_segments.front()->put((*it)->data(), (*it)->size());
    }
    return m_segments.front();
}

bool SegmentBuffer::empty()
{
    return m_segments.empty();
}

const std::list<PBuffer>& SegmentBuffer::segments()
{
    return m_segments;
}

size_t SegmentBuffer::size()
{
    size_t total_size = 0;
    for (const auto& segment : m_segments)
    {
        total_size += segment->size();
    }
    return total_size;
}
