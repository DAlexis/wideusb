#ifndef SX1278DRIVERHAL_HPP
#define SX1278DRIVERHAL_HPP

#include "devices/sx1278/sx1278-driver-base.hpp"
#include "devices/hal.hpp"

class SX1278DriverHAL : public SX1278DriverBase
{
public:
    SX1278DriverHAL(GPIOPin reset,
        GPIOPin dio0,
        GPIOPin nss,
        SPI_HandleTypeDef *spi);

    void init() override;
    void set_nss(int value) override;
    void reset() override;
    void spi_command(uint8_t cmd) override;
    uint8_t spi_read_byte() override;
    void delay_ms(uint32_t msec) override;
    int get_DIO0() override;

private:
    GPIOPin m_reset;
    GPIOPin m_dio0;
    GPIOPin m_nss;
    SPI_HandleTypeDef *m_spi;

};

#endif // SX1278DRIVERHAL_HPP
