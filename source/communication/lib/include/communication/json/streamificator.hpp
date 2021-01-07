#ifndef JSON_STREAMIFICATOR_HPP
#define JSON_STREAMIFICATOR_HPP

#include "communication/channel-layer.hpp"

struct StreamChunkHeaderJSON
{
    uint32_t size = 0;
    uint32_t checksum = 0;
    bool need_checksum = true;

    PBuffer serialize();

    static std::optional<StreamChunkHeaderJSON> parse(const std::string& str);
};

class StreamificatorJSON : public IStreamificator
{
public:
    StreamificatorJSON(bool need_checksum = true);
    bool pack(SerialWriteAccessor& ring_buffer, const PBuffer buffer) override;

private:
    const bool m_need_checksum = true;
};

class DestreamificatorJSON : public IDestreamificator
{
public:
    DestreamificatorJSON(bool need_checksum = true);
    std::optional<PBuffer> unpack(SerialReadAccessor& ring_buffer) override;
    void reset() override;

private:
    enum class State
    {
        waiting_header = 0,
        waiting_buffer
    };

    const bool m_need_checksum = true;

    State m_state = State::waiting_header;
    StreamChunkHeaderJSON m_header;
    size_t m_buffer_bytes_left = 0;
    PBuffer m_data;
};

#endif // JSON_STREAMIFICATOR_HPP
