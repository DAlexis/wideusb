#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "buffer-c.h"

#include <vector>
#include <cstdint>
#include <memory>
#include <list>
#include <limits>

class Buffer;
class RingBuffer;

using PBuffer = std::shared_ptr<Buffer>;

class SerialReadAccessor
{
public:
    virtual void skip(size_t count) = 0;
    virtual void get(uint8_t* buf, size_t size) const = 0;

    virtual size_t size() const = 0;

    virtual uint8_t operator[](size_t pos) const = 0;
    virtual ~SerialReadAccessor() = default;

    virtual bool empty() const;
    virtual void extract(uint8_t* buf, size_t size);

    template <typename T>
    T as(size_t pos)
    {
        T result;
        for (size_t i = 0; i < sizeof(T); i++)
        {
            reinterpret_cast<uint8_t*>(&result)[i] = (*this)[pos + i];
        }
        return result;
    }

    template <typename T>
    SerialReadAccessor& operator>>(T& target)
    {
        extract(reinterpret_cast<uint8_t*>(&target), sizeof(target));
        return *this;
    }
};

class SerialWriteAccessor
{
private:
    class RawStream;

public:
    virtual bool put(const void* data, size_t size) = 0;
    virtual bool put(SerialReadAccessor& buffer, size_t size) = 0;
    virtual bool put(SerialReadAccessor& buffer);
    virtual bool put(SerialReadAccessor&& buffer);

    /**
     * @brief Check if data with given size will fit the container
     * @param size   Data size in bytes
     */
    virtual bool will_fit(size_t size) = 0;

    virtual SerialWriteAccessor& operator<<(SerialReadAccessor&& accessor);
    virtual SerialWriteAccessor& operator<<(SerialReadAccessor& accessor);

    virtual ~SerialWriteAccessor() = default;

    /**
     * @brief Create raw data accessor to trivially serialize anything by copying it
     * @return
     */
    RawStream raw();

private:
    class RawStream
    {
    public:
        RawStream(SerialWriteAccessor& write_accessor) :
            m_write_accessor(write_accessor)
        {}

        template<typename T>
        RawStream& operator<<(const T& variable)
        {
            m_write_accessor.put(reinterpret_cast<const void*>(&variable), sizeof(T));
            return *this;
        }
    private:
        SerialWriteAccessor& m_write_accessor;
    };
};


class Buffer : public std::enable_shared_from_this<Buffer>, public SerialWriteAccessor
{
public:
    static PBuffer create(size_t size = 0, const void* init_data = nullptr);
    static PBuffer create(SerialReadAccessor& data, size_t size);
    static PBuffer create(SerialReadAccessor& data);

    bool put(const void* data, size_t size) override;
    bool put(SerialReadAccessor& accessor, size_t size) override;
    bool will_fit(size_t size) override;

    std::vector<uint8_t>& contents();

    PBuffer clone();

    size_t size() const;
    uint8_t* data();
    const uint8_t* data() const;

    void clear();

    uint8_t& operator[](size_t pos);

    bool operator==(const Buffer& right) const;

private:
    Buffer(size_t size = 0, const void* init_data = nullptr);
    void extend(size_t size);

    std::vector<uint8_t> m_contents;
};

class BufferAccessor : public SerialReadAccessor
{
public:
    BufferAccessor(PBuffer buf, size_t pos = 0, size_t limit = std::numeric_limits<size_t>::max());
    BufferAccessor(const BufferAccessor& buf, size_t pos = 0, size_t limit = std::numeric_limits<size_t>::max());
    void skip(size_t count) override;
    void get(uint8_t* buf, size_t size) const override;
    size_t size() const override;

    uint8_t operator[](size_t pos) const override;

    //BufferAccessor& operator=(const BufferAccessor&) = default;
private:
    PBuffer m_buffer;
    size_t m_offset = 0;
    const size_t m_limit;
};

/*
class Slice : public SerialReadAccessor
{
public:
    Slice(const SerialReadAccessor)
    virtual void skip(size_t count) = 0;
    virtual void get(uint8_t* buf, size_t size) = 0;
    virtual size_t size() const = 0;

    virtual uint8_t operator[](size_t pos) const = 0;
    virtual ~SerialReadAccessor() = default;
};*/

/**
 * @brief The RingBufferClass class is always a serial accessor to itself,
 * because it stores reading pointer and it should be the only one
 */
class RingBuffer : public SerialReadAccessor, public SerialWriteAccessor
{
public:
    RingBuffer(size_t capacity);

    size_t free_space();
    /**
     * @brief Get size of currently stored not readed data
     * @return currently stored data size
     */
    size_t size() const override;

    bool put(const void* src, size_t size) override;
    bool put(SerialReadAccessor& accessor, size_t size) override;
    bool will_fit(size_t size) override;

    void get(uint8_t* buf, size_t size) const override;
    void extract(uint8_t* buf, size_t size) override;
    void skip(size_t size) override;

    bool empty() const override;
    void clear();

    uint8_t operator[](size_t pos) const override;
    uint8_t& operator[](size_t pos);

    RingBufferHandle handle();

private:
    std::vector<uint8_t> m_contents;
    uint32_t m_p_write = 0, m_p_read = 0;
};


class SegmentBuffer
{
public:
    SegmentBuffer(PBuffer buf = nullptr);
    void push_front(PBuffer buf);
    void push_back(PBuffer buf);

    void push_front(SegmentBuffer& buf);
    void push_back(SegmentBuffer& buf);

    PBuffer merge();
    bool empty();
    size_t size();

    /*
    const std::list<PBuffer>& segments() const;

    class Accessor : public SerialReadAccessor
    {
    public:
        Accessor(const SegmentBuffer& seg_buf);
        void skip(size_t count) override;
        void get(uint8_t* buf, size_t size) override;
        size_t size() const override;

        uint8_t operator[](size_t pos) const override;

    private:
        struct SegmentDescriptor
        {
            std::list<PBuffer>::const_iterator segment;
            size_t segment_offset = 0;
        };

        size_t m_total_size;
        size_t m_skip_count = 0;

        size_t find_segment(size_t element_index) const;

        std::vector<SegmentDescriptor> m_segment_descriptors;
    };*/

private:

    std::list<PBuffer> m_segments;
};

#endif // BUFFER_HPP
