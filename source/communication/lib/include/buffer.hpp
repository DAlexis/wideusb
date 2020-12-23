#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>
#include <cstdint>
#include <memory>

#include "buffer-c.h"

class Buffer;
class RingBufferClass;
struct RingBuffer;

using PBuffer = std::shared_ptr<Buffer>;

class Buffer : public std::enable_shared_from_this<Buffer>
{
public:
    static PBuffer create(size_t size = 0, const void* init_data = nullptr);
    static PBuffer create(RingBufferClass& data, size_t size);
    static PBuffer create(RingBufferClass& data);

    Buffer& put(const uint8_t* data, size_t size);
    Buffer& put(RingBufferClass& ring_buffer, size_t size);

    std::vector<uint8_t>& contents();

    PBuffer clone();

    size_t size() const;
    bool empty() const;
    uint8_t* data();
    const uint8_t* data() const;

    void clear();

    uint8_t& operator[](size_t pos);

    Buffer& operator<<(const Buffer& buf);
    Buffer& operator<<(Buffer&& buf);

    //Buffer& operator<<(RingBuffer& ring_buffer);
    Buffer& operator<<(RingBufferClass& ring_buffer);

    template<typename T>
    Buffer& operator<<(const T& variable)
    {
        put(reinterpret_cast<const uint8_t*>(&variable), sizeof(T));
        return *this;
    }

    bool operator==(const Buffer& right) const;

private:
    Buffer(size_t size = 0, const void* init_data = nullptr);
    void extend(size_t size);

    std::vector<uint8_t> m_contents;
};

class ISerialReadAccessor
{
public:
    virtual bool empty() const = 0;
    virtual void skip(size_t count) = 0;
    virtual void get(uint8_t* buf, size_t size) = 0;
    virtual size_t size() const = 0;

    virtual uint8_t operator[](size_t pos) const = 0;
    virtual ~ISerialReadAccessor() = default;
};

class BufferAccessor : public ISerialReadAccessor
{
public:
    BufferAccessor(PBuffer buf, size_t pos = 0);
    bool empty() const override;
    void skip(size_t count) override;
    void get(uint8_t* buf, size_t size) override;
    size_t size() const override;

    uint8_t operator[](size_t pos) const override;
private:
    PBuffer m_buffer;
    size_t m_offset = 0;
};

/**
 * @brief The RingBufferClass class is always a serial accessor to itself,
 * because it stores reading pointer and it should be the only one
 */
class RingBufferClass : public ISerialReadAccessor
{
public:
    RingBufferClass(size_t capacity);

    size_t free_space();
    /**
     * @brief Get size of currently stored not readed data
     * @return currently stored data size
     */
    size_t size() const override;
    void put(const void* src, size_t size);
    void get(uint8_t* buf, size_t size) override;
    void skip(size_t size) override;
    void move_data(RingBufferClass& ring_buffer, size_t size);

    bool empty() const override;
    void clear();

    uint8_t operator[](size_t pos) const override;
    uint8_t& operator[](size_t pos);

    RingBufferClass& operator<<(const Buffer& buffer);

    RingBufferHandle handle();

private:
    std::vector<uint8_t> m_contents;
    uint32_t m_p_write = 0, m_p_read = 0;
};


#endif // BUFFER_HPP
