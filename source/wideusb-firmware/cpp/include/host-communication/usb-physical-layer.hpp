#ifndef USBPHYSICALLAYER_HPP
#define USBPHYSICALLAYER_HPP

#include "wideusb-common/communication/i-physical-layer.hpp"

class USBPhysicalLayer : public IPhysicalLayer
{
public:
    USBPhysicalLayer(size_t ring_buffer_size = 1000);
    SerialReadAccessor& incoming() override;
    void send(PBuffer data) override;
    void on_network_service_connected(NetService& srv) override;

    void receive(const uint8_t* data, size_t size);

    void connect_to_usb_port();

private:
    RingBuffer m_input_ring_buffer;
};

#endif // USBPHYSICALLAYER_HPP
