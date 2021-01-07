#ifndef TRANSPORT_HPP
#define TRANSPORT_HPP

#include "communication/i-transport-layer.hpp"

class TransportLayerBinary : ITransportLayer
{
public:
    std::vector<DecodedSegment> receive(const BufferAccessor& packet) override;

};

#endif // TRANSPORT_HPP
