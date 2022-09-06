#ifndef NRF24L01DRIVERHAL_HPP
#define NRF24L01DRIVERHAL_HPP

#include "devices/nrf24l01/nrf24l01-driver-base.hpp"
#include "devices/hal.hpp"

#include <memory>

class NRF24L01IODriverHal : public NRF24L01IODriverBase
{
public:
    constexpr static int64_t io_timeout = 10;
    using ptr = std::shared_ptr<NRF24L01IODriverHal>;

    NRF24L01IODriverHal(
            GPIOPin chip_select_pin,
            GPIOPin chip_enable_pin,
            GPIOPin irq_pin,
            SPI_HandleTypeDef* spi);

    void set_chip_select(bool state) override;
    void set_chip_enable(bool state) override;
    bool get_irq_pin() override;

    uint8_t transmit_receive(uint8_t byte) override;
    void transmit(uint8_t* data, size_t size) override;
    void receive(uint8_t* data, size_t size) override;

private:
    GPIOPin m_chip_select_pin;
    GPIOPin m_chip_enable_pin;
    GPIOPin m_irq_pin;
    SPI_HandleTypeDef* m_spi;
};

#endif // NRF24L01DRIVERHAL_HPP
