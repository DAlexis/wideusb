#ifndef STREAMIFICATOR_HPP
#define STREAMIFICATOR_HPP

#include "communication/streamification.hpp"

struct StreamChunkHeaderJSON
{
    uint32_t size = 0;
    uint32_t checksum = 0;

    PBuffer serialize();

    static std::optional<StreamChunkHeaderJSON> parse(const std::string& str);
};

class StreamificatorJSON : public IStreamificator
{
public:
    bool pack(RingBuffer& ring_buffer, const PBuffer buffer) override;

private:
};

class DestreamificatorJSON : public IDestreamificator
{
public:
    std::optional<PBuffer> unpack(SerialReadAccessor& ring_buffer) override;
    void reset() override;

private:
    enum class State
    {
        waiting_header = 0,
        waiting_buffer
    };

    State m_state = State::waiting_header;
    StreamChunkHeaderJSON m_header;
    size_t m_buffer_bytes_left = 0;
    PBuffer m_data;
};

#endif // STREAMIFICATOR_HPP
