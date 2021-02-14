#include "communication/i-physical-layer.hpp"

PhysicalLayerBuffer::PhysicalLayerBuffer(size_t incoming_capacity) :
    m_ring_buffer(incoming_capacity)
{
}

SerialReadAccessor& PhysicalLayerBuffer::incoming()
{
    return m_ring_buffer;
}

void PhysicalLayerBuffer::send(const PBuffer data)
{
    m_out_queue.push(data);
}

PBuffer PhysicalLayerBuffer::out_next()
{
    if (m_out_queue.empty())
        return nullptr;
    PBuffer result = m_out_queue.front();
    m_out_queue.pop();
    return result;
}

void PhysicalLayerBuffer::in_next(const void* data, size_t size)
{
    m_ring_buffer.put(data, size);
}

size_t PhysicalLayerBuffer::out_queue_size()
{
    return m_out_queue.size();
}


void loop_back(PhysicalLayerBuffer& phys_layer)
{
    PBuffer data;
    while (nullptr != (data = phys_layer.out_next()))
    {
        phys_layer.in_next(data->data(), data->size());
    }
}

void exchange_data(std::vector<PhysicalLayerBuffer*> phys_layers)
{
    for (auto source : phys_layers)
    {
        PBuffer data = source->out_next();
        for (auto dest : phys_layers)
        {
            dest->in_next(data->data(), data->size());
        }
    }
}
