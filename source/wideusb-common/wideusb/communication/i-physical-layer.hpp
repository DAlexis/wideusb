#ifndef IPHYSICALLAYER_HPP
#define IPHYSICALLAYER_HPP

#include "wideusb/buffer.hpp"

#include <queue>

class NetService;

class IPhysicalLayer
{
public:
    virtual SerialReadAccessor& incoming() = 0;
    virtual void send(PBuffer data) = 0;
    virtual ~IPhysicalLayer() = default;
};

class PhysicalLayerBuffer : public IPhysicalLayer
{
public:
    PhysicalLayerBuffer(size_t incoming_capacity);

    SerialReadAccessor& incoming() override;
    void send(PBuffer data) override;

    PBuffer out_next();
    void in_next(const void* data, size_t size);
    size_t out_queue_size();

private:
    RingBuffer m_ring_buffer;
    std::queue<PBuffer> m_out_queue;
};

void loop_back(PhysicalLayerBuffer& phys_layer);

void exchange_data(std::vector<PhysicalLayerBuffer*> phys_layers);

#endif // IPHYSICALLAYER_HPP

