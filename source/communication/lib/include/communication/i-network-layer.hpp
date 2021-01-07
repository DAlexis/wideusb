#ifndef INETWORKLAYER_HPP
#define INETWORKLAYER_HPP

#include "communication/network-base.hpp"
#include "buffer.hpp"

#include <vector>

struct DecodedPacket
{
    DecodedPacket(Address sender, Address receiver, uint8_t ttl, const BufferAccessor& packet) :
        sender(sender), receiver(receiver), ttl(ttl), packet(packet)
    { }
    Address sender;
    Address receiver;
    uint8_t ttl;
    BufferAccessor packet;
};

class INetworkLayer
{
public:
    virtual std::vector<DecodedPacket> decode(const BufferAccessor& frame) = 0;
    virtual void encode(SegmentBuffer& packet, Address sender, Address receiver, uint8_t ttl) = 0;

    virtual ~INetworkLayer() = default;
};


#endif // INETWORKLAYER_HPP
