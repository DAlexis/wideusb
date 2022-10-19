#ifndef INETWORKLAYER_HPP
#define INETWORKLAYER_HPP

#include "wideusb/communication/network-types.hpp"
#include "wideusb/buffer.hpp"

#include <vector>

struct NetworkOptions
{
    NetworkOptions(Address sender, Address receiver, uint32_t id, uint8_t ttl, bool is_broadcast) :
        sender(sender), receiver(receiver), id(id), ttl(ttl), is_broadcast(is_broadcast)
    { }

    Address sender;
    Address receiver;
    uint32_t id;
    uint8_t ttl;
    bool is_broadcast; ///< Retransmit package even if it was received

    bool operator<(const NetworkOptions& right) const
    {
        if (sender != right.sender)
            return sender < right.sender;
        if (receiver != right.receiver)
            return receiver < right.receiver;
        if (ttl != right.ttl)
            return ttl < right.ttl;
        if (id != right.id)
            return id < right.id;
        if (is_broadcast != right.is_broadcast)
            return is_broadcast < right.is_broadcast;

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
