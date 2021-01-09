#ifndef ICHANNELLEVEL_HPP
#define ICHANNELLEVEL_HPP

#include "buffer.hpp"

#include <vector>

struct DecodedFrame
{
    DecodedFrame(const BufferAccessor& accessor) :
        frame(accessor)
    { }
    BufferAccessor frame;
};

class IChannelLayer
{
public:
    virtual std::vector<DecodedFrame> decode(RingBuffer& ring_buffer) = 0;
    virtual void encode(SegmentBuffer& frame) = 0;
    virtual ~IChannelLayer() = default;
};

#endif // ICHANNELLEVEL_HPP
