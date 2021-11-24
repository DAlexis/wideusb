#ifndef TRANSPORT_HPP
#define TRANSPORT_HPP

#include "wideusb/communication/i-transport-layer.hpp"

#pragma pack(push, 1)
// TODO: Make variadic header for bytes economy
struct SegmentHeader
{
    uint32_t port = 0;
    uint8_t flags = 0;

    uint32_t segment_id = 0;
    uint32_t ack_id = 0;
    uint32_t size = 0;
};
#pragma pack(pop)

class TransportLayerBinary : public ITransportLayer
{
public:
    std::vector<DecodedSegment> decode(const BufferAccessor& packet) override;
    void encode(SegmentBuffer& segment, uint32_t port, uint32_t segment_id, bool need_ack, bool make_ack, uint32_t ack_id) override;
};

#endif // TRANSPORT_HPP
