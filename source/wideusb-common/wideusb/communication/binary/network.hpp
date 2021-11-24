#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "wideusb/communication/i-network-layer.hpp"

class NetworkLayerBinary : public INetworkLayer
{
public:
    std::vector<DecodedPacket> decode(const BufferAccessor& frame) override;
    void encode(SegmentBuffer& packet, const NetworkOptions& options) override;
};

#endif // NETWORK_HPP
