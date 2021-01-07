#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "communication/i-channel-level.hpp"

#pragma pack(push, 1)
struct ChannelHeader
{
    constexpr static uint32_t magic_number_value = 0xABCD;
    uint32_t magic = magic_number_value;
    uint32_t size = 0;
    uint32_t checksum = 0;
};
#pragma pack(pop)

class ChannelLayerBinary : public IChannelLayer
{
public:
    ChannelLayerBinary(size_t buffer_capacity);
    std::vector<DecodedFrame> decode(const uint8_t* data, size_t size) override;
    void encode(SegmentBuffer& frame) override;

private:
    enum class State
    {
        waiting_header = 0,
        waiting_buffer
    };

    State m_state = State::waiting_header;
    ChannelHeader m_header;

    struct DecodingInstance
    {
        size_t body_begin = 0;
        ChannelHeader header;
    };

    PBuffer decode_single();
    PBuffer find_sucessful_instance();
    void find_next_headers();

    RingBuffer m_ring_buffer;

    std::list<DecodingInstance> m_decoding_instances;
    size_t m_header_search_pos = 0;
};

#endif // CHANNEL_HPP
