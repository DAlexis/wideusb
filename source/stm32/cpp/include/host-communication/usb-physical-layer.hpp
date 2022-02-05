#ifndef USBPHYSICALLAYER_HPP
#define USBPHYSICALLAYER_HPP

#include "wideusb/communication/i-physical-layer.hpp"
#include "os/cpp-freertos.hpp"
#include "os/async-worker.hpp"

class USBPhysicalLayer : public IPhysicalLayer
{
public:
    USBPhysicalLayer(AsyncWorker& async_worker, size_t ring_buffer_size = 1000);
    SerialReadAccessor& incoming() override;
    void send(PBuffer data) override;
    void set_on_data_callback(std::function<void(void)> callback) override;

    void receive(const uint8_t* data, size_t size);

    void connect_to_usb_port();

private:
    void notification_thread_body();

    RingBuffer m_input_ring_buffer;
    std::function<void(void)> m_callback;
    AsyncWorker& m_async_worker;
};

#endif // USBPHYSICALLAYER_HPP
