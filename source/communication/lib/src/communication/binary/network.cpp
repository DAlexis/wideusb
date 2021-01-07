#include "communication/binary/network.hpp"

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

        result.emplace_back(DecodedPacket(header.sender, header.receiver, header.ttl, BufferAccessor(current_accessor, 0, header.size)));
        current_accessor.skip(header.size);
    }
    return result;
}

void NetworkLayerBinary::encode(SegmentBuffer& packet, Address sender, Address receiver, uint8_t ttl)
{
    PacketHeader header;
    header.receiver = receiver;
    header.sender = sender;
    header.ttl = ttl;
    header.size = packet.size();
    packet.push_front(Buffer::create(sizeof(header), &header));
}
