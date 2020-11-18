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
    static PBuffer create(size_t size = 0, const uint8_t* init_data = nullptr);

    Buffer& append(const uint8_t* data, size_t size);
    Buffer& append(RingBuffer& data, size_t size);

    std::vector<uint8_t>& contents();

    PBuffer clone();

    size_t size() const;
    uint8_t* data();
    const uint8_t* data() const;

    void clear();

    Buffer& operator<<(const Buffer& buf);
    Buffer& operator<<(Buffer&& buf);
private:
    Buffer(size_t size = 0, const uint8_t* init_data = nullptr);

    std::vector<uint8_t> m_contents;
};



#endif // BUFFER_HPP
