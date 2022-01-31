#ifndef USBPHYSICALLAYER_HPP
#define USBPHYSICALLAYER_HPP

#include "wideusb/communication/i-physical-layer.hpp"
#include "os/cpp-freertos.hpp"

class USBPhysicalLayer : public IPhysicalLayer
{
public:
    USBPhysicalLayer(size_t ring_buffer_size = 1000);
    SerialReadAccessor& incoming() override;
    void send(PBuffer data) override;
    void set_on_data_callback(std::function<void(void)> callback) override;

    void receive(const uint8_t* data, size_t size);

    void connect_to_usb_port();

private:
    void notification_thread_body();

    RingBuffer m_input_ring_buffer;
    std::function<void(void)> m_callback;
    os::Thread m_notification_thread{[this](){ notification_thread_body(); }, "usb-notify", 128};
};

#endif // USBPHYSICALLAYER_HPP
