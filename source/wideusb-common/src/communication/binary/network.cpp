#include "wideusb-common/communication/binary/network.hpp"

#pragma pack(push, 1)
struct PacketHeader
{
    Address sender = 0;
    Address receiver = 0;
    uint8_t ttl = 5;
    uint32_t size = 0;
};
#pragma pack(pop)


std::vector<DecodedPacket> NetworkLayerBinary::decode(const BufferAccessor& frame)
{
    std::vector<DecodedPacket> result;
    BufferAccessor current_accessor(frame);
    while (current_accessor.size() > sizeof(PacketHeader))
    {
        PacketHeader header;
        current_accessor >> header;
        if (header.size > current_accessor.size())
            break;

        NetworkOptions opts(header.sender, header.receiver, header.ttl);
        result.emplace_back(opts, BufferAccessor(current_accessor, 0, header.size));
        current_accessor.skip(header.size);
    }
    return result;
}

void NetworkLayerBinary::encode(SegmentBuffer& packet, const NetworkOptions& options)
{
    PacketHeader header;
    header.receiver = options.receiver;
    header.sender = options.sender;
    header.ttl = options.ttl;
    header.size = packet.size();
    packet.push_front(Buffer::create(sizeof(header), &header));
}
