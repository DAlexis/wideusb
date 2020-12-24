#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>
#include <cstdint>
#include <memory>

#include "buffer-c.h"

class Buffer;
class RingBuffer;

using PBuffer = std::shared_ptr<Buffer>;

class SerialReadAccessor
{
public:
    virtual bool empty() const
    { return size() == 0; }
    virtual void skip(size_t count) = 0;
    virtual void get(uint8_t* buf, size_t size) = 0;
    virtual size_t size() const = 0;

    virtual uint8_t operator[](size_t pos) const = 0;
    virtual ~SerialReadAccessor() = default;
};

class SerialWriteAccessor
{
private:
    class RawStream;

public:
    virtual void put(const void* data, size_t size) = 0;
    virtual void put(SerialReadAccessor& buffer, size_t size) = 0;

    virtual SerialWriteAccessor& operator<<(SerialReadAccessor&& accessor);
    virtual SerialWriteAccessor& operator<<(SerialReadAccessor& accessor);

    virtual ~SerialWriteAccessor() = default;

    /**
     * @brief Create raw data accessor to trivially serialize anything by copying it
     * @return
     */
    RawStream raw() { return RawStream(*this); }

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
    static PBuffer create(RingBuffer& data, size_t size);
    static PBuffer create(RingBuffer& data);

    void put(const void* data, size_t size) override;
    void put(SerialReadAccessor& accessor, size_t size) override;

    std::vector<uint8_t>& contents();

    PBuffer clone();

    size_t size() const;
    uint8_t* data();
    const uint8_t* data() const;

    void clear();

    uint8_t& operator[](size_t pos);

/*
    */

    bool operator==(const Buffer& right) const;

private:
    Buffer(size_t size = 0, const void* init_data = nullptr);
    void extend(size_t size);

    std::vector<uint8_t> m_contents;
};

class BufferAccessor : public SerialReadAccessor
{
public:
    BufferAccessor(PBuffer buf, size_t pos = 0);
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

    void put(const void* src, size_t size) override;
    void put(SerialReadAccessor& accessor, size_t size) override;

    void get(uint8_t* buf, size_t size) override;
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


#endif // BUFFER_HPP
