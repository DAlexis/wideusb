#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "communication/i-network-layer.hpp"

#pragma pack(push, 1)
struct PacketHeader
{
    Address sender = 0;
    Address receiver = 0;
    uint8_t ttl = 5;
    uint32_t size = 0;
};
#pragma pack(pop)

class NetworkLayerBinary : public INetworkLayer
{
public:
    std::vector<DecodedPacket> decode(const BufferAccessor& frame) override;
    void encode(SegmentBuffer& packet, Address sender, Address receiver, uint8_t ttl) override;
};

#endif // NETWORK_HPP
