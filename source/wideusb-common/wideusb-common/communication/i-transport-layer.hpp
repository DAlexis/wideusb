#ifndef ITRANSPORTLAYER_HPP
#define ITRANSPORTLAYER_HPP

#include "wideusb-common/buffer.hpp"

struct DecodedSegment
{
    DecodedSegment(uint32_t port, uint8_t flags, uint32_t segment_id, uint32_t ack_id, const BufferAccessor& segment) :
        port(port), flags(flags), segment_id(segment_id), ack_for_segment_id(ack_id), segment(segment)
    { }

    struct Flags
    {
        constexpr static uint8_t need_ack = 1;
        constexpr static uint8_t is_ack   = 2;
    };

    uint32_t port;
    uint8_t flags;
    uint32_t segment_id;
    uint32_t ack_for_segment_id;
    BufferAccessor segment;
};

class ITransportLayer
{
public:
    virtual std::vector<DecodedSegment> decode(const BufferAccessor& packet) = 0;
    virtual void encode(SegmentBuffer& segment, uint32_t port, uint32_t segment_id, bool need_ack = false, bool make_ack = false, uint32_t ack_id = 0) = 0;

    virtual ~ITransportLayer() = default;
};
/*
struct TransmissionPolicy
{

};*/

#endif // ITRANSPORTLAYER_HPP
