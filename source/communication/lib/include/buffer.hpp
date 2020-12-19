#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>
#include <cstdint>
#include <memory>

class Buffer;
struct RingBuffer;

using PBuffer = std::shared_ptr<Buffer>;

class Buffer : public std::enable_shared_from_this<Buffer>
{
public:
    static PBuffer create(size_t size = 0, const void* init_data = nullptr);
    static PBuffer create(RingBuffer& data, size_t size);
    static PBuffer create(RingBuffer& data);

    Buffer& append(const uint8_t* data, size_t size);
    Buffer& append(RingBuffer& data, size_t size);

    std::vector<uint8_t>& contents();

    PBuffer clone();

    size_t size() const;
    uint8_t* data();
    const uint8_t* data() const;

    void clear();

    /// Dump data to ring buffer supposing that it has enough space
    const Buffer& operator>>(RingBuffer& target) const;

    Buffer& operator<<(const Buffer& buf);
    Buffer& operator<<(Buffer&& buf);

    Buffer& operator<<(RingBuffer& ring_buffer);

    template<typename T>
    Buffer& operator<<(const T& variable)
    {
        append(reinterpret_cast<const uint8_t*>(&variable), sizeof(T));
        return *this;
    }

    bool operator==(const Buffer& right) const;

private:
    Buffer(size_t size = 0, const void* init_data = nullptr);

    std::vector<uint8_t> m_contents;
};


class RingBufferClass
{
public:
    RingBufferClass(size_t capacity);

    size_t free_space();
    size_t data_size();
    void put(const uint8_t* buf, size_t size);
    void get(uint8_t* buf, size_t size);
    void skip(size_t size);

    uint8_t* operator[](size_t pos);

private:
    std::vector<uint8_t> m_contents;
    uint32_t m_p_write = 0, m_p_read = 0;
};


#endif // BUFFER_HPP
