#include "host-communication/usb-physical-layer.hpp"
#include "usbd_cdc_if.h"

#include <string>

static USBPhysicalLayer* usb_handler = nullptr;

void usbd_rx_handler(const uint8_t* buffer, uint32_t size)
{
    if (usb_handler)
        usb_handler->receive(buffer, size);
}

USBPhysicalLayer::USBPhysicalLayer(AsyncWorker& async_worker, size_t ring_buffer_size) :
    m_input_ring_buffer(ring_buffer_size), m_async_worker(async_worker)
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

void USBPhysicalLayer::set_on_data_callback(std::function<void(void)> callback)
{
    m_callback = callback;
}

void USBPhysicalLayer::receive(const uint8_t* data, size_t size)
{
    // Here we are in interrupt context
    m_input_ring_buffer.put(data, std::min(size, m_input_ring_buffer.free_space()));
    m_async_worker.post(m_callback);
}

void USBPhysicalLayer::connect_to_usb_port()
{
    usb_handler = this;
}

