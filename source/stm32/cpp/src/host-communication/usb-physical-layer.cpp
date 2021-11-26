#include "host-communication/usb-physical-layer.hpp"

#include "usbd-input-handle.h"
#include "usbd_cdc_if.h"

static USBPhysicalLayer* usb_handler = nullptr;

void usbd_rx_handler(const uint8_t* buffer, uint32_t size)
{
    if (usb_handler)
        usb_handler->receive(buffer, size);
}

USBPhysicalLayer::USBPhysicalLayer(size_t ring_buffer_size) :
    m_input_ring_buffer(ring_buffer_size)
{
    connect_to_usb_port();
}


SerialReadAccessor& USBPhysicalLayer::incoming()
{
    return m_input_ring_buffer;
}

void USBPhysicalLayer::send(PBuffer data)
{
    CDC_Transmit_FS(data->data(), data->size());
}

void USBPhysicalLayer::on_network_service_connected(NetService& srv)
{
    // nothing
}

void USBPhysicalLayer::receive(const uint8_t* data, size_t size)
{
    m_input_ring_buffer.put(data, std::min(size, m_input_ring_buffer.free_space()));
}

void USBPhysicalLayer::connect_to_usb_port()
{
    usb_handler = this;
}
