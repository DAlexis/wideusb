#ifndef IPHYSICALLAYER_HPP
#define IPHYSICALLAYER_HPP

#include "buffer.hpp"

#include <queue>

class IPhysicalLayer
{
public:
    virtual RingBuffer& incoming() = 0;
    virtual void send(PBuffer data) = 0;
    virtual ~IPhysicalLayer() = default;
};

class PhysicalLayerBuffer : public IPhysicalLayer
{
public:
    PhysicalLayerBuffer(size_t incoming_capacity);

    RingBuffer& incoming() override;
    void send(PBuffer data) override;

    PBuffer out_next();
    void in_next(const void* data, size_t size);

private:
    RingBuffer m_ring_buffer;
    std::queue<PBuffer> m_out_queue;
};

#endif // IPHYSICALLAYER_HPP
