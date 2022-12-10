#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include <list>
#include <limits>
#include <optional>
#include <cstring>

class Buffer;
class RingBuffer;

using PBuffer = std::shared_ptr<Buffer>;

struct MemBlock
{
    const uint8_t* data = nullptr;
    size_t size = 0;
};

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
    PBuffer extract_buf(size_t extraction_size);
    PBuffer extract_buf();

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
     * @return temporary object that redirect data to current SerialWriteAccessor
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
    template<typename T>
    static PBuffer serialize(const T& data)
    {
        return create(sizeof(data), &data);
    }

    bool put(const void* data, size_t size) override;
    bool put(SerialReadAccessor& accessor, size_t size) override;
    bool will_fit(size_t size) override;

    std::vector<uint8_t>& contents();

    PBuffer clone() const;

    size_t size() const;
    uint8_t* data();
    const uint8_t* data() const;

    void clear();

    uint8_t& at(size_t pos);
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
    const uint8_t* data() const;

    uint8_t operator[](size_t pos) const override;

    //BufferAccessor& operator=(const BufferAccessor&) = default;
private:
    PBuffer m_buffer;
    size_t m_offset = 0;
    const size_t m_limit;
};

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
    bool put(PBuffer buf);
    bool will_fit(size_t size) override;

    void get(uint8_t* buf, size_t size) const override;
    void extract(uint8_t* buf, size_t size) override;
    void skip(size_t size) override;

    /**
     * @brief Get largest avaliable continious data block with its size
     *        May be bug here (old DAC case)??? Not used now
     * @param buf   OUT: Pointer to block
     * @param size  OUT: Block size
     */
    MemBlock get_continious_block(size_t size) const;

    bool empty() const override;
    void clear();

    uint8_t operator[](size_t pos) const override;
    uint8_t& operator[](size_t pos);

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

    const std::list<PBuffer>& segments();

private:

    std::list<PBuffer> m_segments;
};

template<typename T>
bool buffer_size_compatible(const PBuffer buf)
{
    return sizeof(T) == buf->size();
}

template<typename T>
std::optional<T> try_interpret_buffer_no_magic(const PBuffer buf)
{
    if (!buffer_size_compatible<T>(buf))
        return std::nullopt;

    T result;
    memcpy(&result, buf->data(), sizeof(T));
    return result;
}


template<typename T>
std::optional<T> try_interpret_buffer_magic(const PBuffer buf)
{
    if (!buffer_size_compatible<T>(buf))
        return std::nullopt;

    uint8_t try_magic = *reinterpret_cast<const uint8_t*>(buf->data());
    if (try_magic != T::magic)
        return std::nullopt;

    T result;
    memcpy(&result, buf->data(), sizeof(T));
    return result;
}

#endif // BUFFER_HPP
