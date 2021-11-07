#ifndef INETWORKLAYER_HPP
#define INETWORKLAYER_HPP

#include "wideusb-common/communication/network-types.hpp"
#include "wideusb-common/buffer.hpp"

#include <vector>

struct NetworkOptions
{
    NetworkOptions(Address sender, Address receiver, uint8_t ttl = 10) :
        sender(sender), receiver(receiver), ttl(ttl)
    { }

    Address sender;
    Address receiver;
    uint8_t ttl;

    bool operator<(const NetworkOptions& right) const
    {
        if (sender != right.sender)
            return sender < right.sender;
        if (receiver != right.receiver)
            return receiver < right.receiver;
        if (ttl != right.ttl)
            return ttl < right.ttl;
        return false;
    }
};

struct DecodedPacket
{
    DecodedPacket(const NetworkOptions& options, const BufferAccessor& packet) :
        options(options), packet(packet)
    { }

    NetworkOptions options;

    BufferAccessor packet;
};

class INetworkLayer
{
public:
    virtual std::vector<DecodedPacket> decode(const BufferAccessor& frame) = 0;
    virtual void encode(SegmentBuffer& packet, const NetworkOptions& options) = 0;

    virtual ~INetworkLayer() = default;
};


#endif // INETWORKLAYER_HPP
