#ifndef TRANSPORT_HPP
#define TRANSPORT_HPP

#include "wideusb/communication/i-transport-layer.hpp"

class TransportLayerBinary : public ITransportLayer
{
public:
    std::vector<DecodedSegment> decode(const BufferAccessor& packet) override;
    void encode(SegmentBuffer& segment, uint32_t port, uint32_t segment_id, bool need_ack, bool make_ack, uint32_t ack_id) override;
};

#endif // TRANSPORT_HPP
