#include "host-communication/usb-physical-layer.hpp"

USBPhysicalLayer::USBPhysicalLayer(size_t ring_buffer_size) :
    m_input_ring_buffer(ring_buffer_size)
{

}


SerialReadAccessor& USBPhysicalLayer::incoming()
{
    return m_input_ring_buffer;
}

void USBPhysicalLayer::send(PBuffer data)
{
}

void USBPhysicalLayer::receive(const uint8_t* data, size_t size)
{
}
