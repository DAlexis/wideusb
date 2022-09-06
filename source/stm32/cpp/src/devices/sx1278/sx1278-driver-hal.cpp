#include "devices/sx1278/sx1278-driver-hal.hpp"

SX1278DriverHAL::SX1278DriverHAL(GPIOPin reset,
    GPIOPin dio0,
    GPIOPin nss,
    SPI_HandleTypeDef *spi) :
    m_reset(reset),
    m_dio0(dio0),
    m_nss(nss),
    m_spi(spi)
{
}

void SX1278DriverHAL::init()
{
    set_nss(1);
    HAL_GPIO_WritePin(m_reset.gpio, m_reset.pin, GPIO_PIN_SET);
}

void SX1278DriverHAL::set_nss(int value)
{
    HAL_GPIO_WritePin(m_nss.gpio, m_nss.pin, (value == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void SX1278DriverHAL::reset()
{
    set_nss(1);
    HAL_GPIO_WritePin(m_reset.gpio, m_reset.pin, GPIO_PIN_RESET);

    delay_ms(1);

    HAL_GPIO_WritePin(m_reset.gpio, m_reset.pin, GPIO_PIN_SET);

    delay_ms(100);
}

void SX1278DriverHAL::spi_command(uint8_t cmd)
{
    set_nss(0);
    HAL_SPI_Transmit(m_spi, &cmd, 1, 1000);
    while (HAL_SPI_GetState(m_spi) != HAL_SPI_STATE_READY)
        ;
}

uint8_t SX1278DriverHAL::spi_read_byte()
{
    uint8_t txByte = 0x00;
    uint8_t rxByte = 0x00;

    set_nss(0);
    HAL_SPI_TransmitReceive(m_spi, &txByte, &rxByte, 1, 1000);
    while (HAL_SPI_GetState(m_spi) != HAL_SPI_STATE_READY)
        ;
    return rxByte;
}

void SX1278DriverHAL::delay_ms(uint32_t msec)
{
    HAL_Delay(msec);
}

int SX1278DriverHAL::get_DIO0()
{
    return (HAL_GPIO_ReadPin(m_dio0.gpio, m_dio0.pin) == GPIO_PIN_SET);
}
