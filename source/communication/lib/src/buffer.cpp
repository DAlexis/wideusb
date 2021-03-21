#include "buffer.hpp"

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

/*
const std::list<PBuffer>& SegmentBuffer::segments() const
{
    return m_segments;
}
*/
/*
SegmentBuffer::Accessor::Accessor(const SegmentBuffer& seg_buf)
{
    size_t total_offset = 0;
    m_segment_descriptors.clear();
    size_t segment_index = 0;
    for (auto it = seg_buf.m_segments.begin(); it != seg_buf.m_segments.end(); total_offset += (*it)->size(), ++it)
    {
        m_segment_descriptors.push_back(SegmentDescriptor());
        m_segment_descriptors.back().segment = it;
        m_segment_descriptors.back().segment_offset = total_offset;
    }
    const auto& last_segment = m_segment_descriptors.back();
    m_total_size = m_segment_descriptors.back().segment_offset
            + (*m_segment_descriptors.back().segment)->size();
}

void SegmentBuffer::Accessor::skip(size_t count)
{
    m_skip_count += count;
}

void SegmentBuffer::Accessor::get(uint8_t* buf, size_t size)
{
    size_t pos = m_skip_count;
    size_t first_segment = find_segment(pos);
    size_t segment_data_begin = pos - m_segment_descriptors[first_segment].segment_offset;
    const PBuffer segment_buf = *m_segment_descriptors[first_segment].segment;
    size_t segment_tail = segment_buf->size() - segment_data_begin;
    if (segment_tail >= size)
    {
        memcpy(buf, segment_buf->data() + segment_data_begin, size);
    } else {
        memcpy(buf, segment_buf->data() + segment_data_begin, segment_tail);
        size -= segment_tail;

        for (size_t seg_index = first_segment + 1; seg_index < m_segment_descriptors.size(); seg_index++)
        {
            // TODO
            TODO
        }
    }

    for (size_t i = find_segment(pos); i < m_segment_descriptors.size(); i++)
    {
        size_t segment_data_begin = pos - m_segment_descriptors[i].segment_offset;
        const PBuffer segment_buf = *m_segment_descriptors[i].segment;
        size_t segment_data_size = segment_buf->size();

        if (segment_data_size - segment_data_begin >= size)
        {
            memcpy(buf, segment_buf->data() + segment_data_begin, size);
            break;
        } else {
            memcpy(buf, segment_buf->data() + segment_data_begin, size);
        }
    }

    m_skip_count += size;
}

size_t SegmentBuffer::Accessor::size() const
{
    return m_total_size - m_skip_count;
}

uint8_t SegmentBuffer::Accessor::operator[](size_t pos) const
{
    pos += m_skip_count;
    const auto& seg = m_segment_descriptors[find_segment(pos)];
    pos -= seg.segment_offset;
    return (**seg.segment)[pos];
}

size_t SegmentBuffer::Accessor::find_segment(size_t element_index) const
{
    size_t seg_index = 0;
    for (; seg_index < m_segment_descriptors.size(); seg_index++)
    {
        if (element_index > m_segment_descriptors[seg_index].segment_offset)
            break;
    }
    return seg_index;
}
*/
