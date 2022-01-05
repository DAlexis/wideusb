#include "wideusb/communication/binary/transport.hpp"

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

std::vector<DecodedSegment> TransportLayerBinary::decode(const BufferAccessor& packet)
{
    std::vector<DecodedSegment> result;
    BufferAccessor current_accessor(packet);
    while (current_accessor.size() >= sizeof(SegmentHeader))
    {
        SegmentHeader header;
        current_accessor >> header;
        if (header.size > current_accessor.size())
            break;

        result.emplace_back(DecodedSegment(header.port, header.flags, header.segment_id, header.ack_id, BufferAccessor(current_accessor, 0, header.size)));
        current_accessor.skip(header.size);
    }
    return result;
}

void TransportLayerBinary::encode(SegmentBuffer& segment, uint32_t port, uint32_t segment_id, bool need_ack, bool make_ack, uint32_t ack_id)
{
    SegmentHeader header;
    header.port = port;
    header.segment_id = segment_id;
    header.ack_id = ack_id;
    header.size = segment.size();
    if (make_ack)
        header.flags |= DecodedSegment::Flags::is_ack;

    if (need_ack)
        header.flags |= DecodedSegment::Flags::need_ack;


    segment.push_front(Buffer::create(sizeof(header), &header));
}
