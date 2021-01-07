#ifndef ITRANSPORTLAYER_HPP
#define ITRANSPORTLAYER_HPP


#include "buffer.hpp"

struct DecodedSegment
{
    uint32_t port;
    BufferAccessor segment;

    SegmentBuffer response;
};

class ITransportLayer
{
public:
    virtual std::vector<DecodedSegment> receive(const BufferAccessor& packet) = 0;

    virtual ~ITransportLayer() = default;
};

#endif // ITRANSPORTLAYER_HPP
