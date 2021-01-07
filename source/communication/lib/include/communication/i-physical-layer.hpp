#ifndef IPHYSICALLAYER_HPP
#define IPHYSICALLAYER_HPP

#include "buffer.hpp"

class IPhysicalLevel
{
public:
    virtual void set_target_buffer(RingBuffer& ring_buffer) = 0;
    virtual void send(const PBuffer data) = 0;
    virtual ~IPhysicalLevel() = default;
};


#endif // IPHYSICALLAYER_HPP
