#ifndef SX1278PHYSICALLAYER_HPP
#define SX1278PHYSICALLAYER_HPP

#include "wideusb/communication/i-physical-layer.hpp"
#include "os/cpp-freertos.hpp"
#include "os/async-worker.hpp"

class SX1278Device;
class SX1278DriverBase;

class SX1278PhysicalLayer : public IPhysicalLayer
{
public:
    SX1278PhysicalLayer(std::shared_ptr<SX1278DriverBase> driver, size_t ring_buffer_size = 1000);

    void run();

    SerialReadAccessor& incoming() override;
    void send(PBuffer data) override;
    void set_on_data_callback(std::function<void(void)> callback) override;

private:
    constexpr static int message_len = 16;
    void service_loop();
    void switch_to_rx();
    void switch_to_tx();

    std::unique_ptr<SX1278Device> m_sx1278;
    RingBuffer m_input_ring_buffer;
    RingBuffer m_output_ring_buffer;
    std::function<void(void)> m_callback;
    std::vector<uint8_t> m_tx_buffer;
    os::Thread m_service_thread{[this](){ service_loop(); }, "sx1278", 512};
};


#endif // SX1278PHYSICALLAYER_HPP
