#ifndef NRF24L01DRIVER_HPP
#define NRF24L01DRIVER_HPP

#include "devices/nrf24l01-driver-base.hpp"

#include <memory>

class NRF24L01IODriverImpl : public NRF24L01IODriverBase
{
public:
    constexpr static int64_t io_timeout = 10;
    using ptr = std::shared_ptr<NRF24L01IODriverImpl>;

    static std::shared_ptr<NRF24L01IODriverImpl> get();

    void set_chip_select(bool state) override;
    void set_chip_enable(bool state) override;
    bool get_irq_pin() override;

    uint8_t transmit_receive(uint8_t byte) override;
    void transmit(uint8_t* data, size_t size) override;
    void receive(uint8_t* data, size_t size) override;

private:
    NRF24L01IODriverImpl();
    void on_tx_rx_ok_interrupt();
    void on_tx_rx_error_interrupt();

    bool wait_for_done(int64_t timeout_ms);

    volatile bool m_done = true;

    static ptr impl;
};

#endif // NRF24L01DRIVER_HPP
