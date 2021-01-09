#include "communication/i-physical-layer.hpp"

PhysicalLayerBuffer::PhysicalLayerBuffer(size_t incoming_capacity) :
    m_ring_buffer(incoming_capacity)
{
}

RingBuffer& PhysicalLayerBuffer::incoming()
{
    return m_ring_buffer;
}

void PhysicalLayerBuffer::send(const PBuffer data)
{
    m_out_queue.push(data);
}

PBuffer PhysicalLayerBuffer::out_next()
{
    PBuffer result = m_out_queue.front();
    m_out_queue.pop();
    return result;
}

void PhysicalLayerBuffer::in_next(const void* data, size_t size)
{
    m_ring_buffer.put(data, size);
}
