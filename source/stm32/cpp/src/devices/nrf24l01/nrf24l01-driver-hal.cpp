#include "devices/nrf24l01/nrf24l01-driver-hal.hpp"

NRF24L01IODriverHal::NRF24L01IODriverHal(
        GPIOPin chip_select_pin,
        GPIOPin chip_enable_pin,
        GPIOPin irq_pin,
        SPI_HandleTypeDef* spi) :
    m_chip_select_pin(chip_select_pin),
    m_chip_enable_pin(chip_enable_pin),
    m_irq_pin(irq_pin),
    m_spi(spi)
{
}

void NRF24L01IODriverHal::set_chip_select(bool state)
{
    HAL_GPIO_WritePin(m_chip_select_pin.gpio, m_chip_select_pin.pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void NRF24L01IODriverHal::set_chip_enable(bool state)
{
    HAL_GPIO_WritePin(m_chip_enable_pin.gpio, m_chip_enable_pin.pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool NRF24L01IODriverHal::get_irq_pin()
{
    return HAL_GPIO_ReadPin(m_irq_pin.gpio, m_irq_pin.pin) == GPIO_PIN_SET;
}

uint8_t NRF24L01IODriverHal::transmit_receive(uint8_t byte)
{
    uint8_t result = 0;
    HAL_StatusTypeDef res = HAL_SPI_TransmitReceive(m_spi, &byte, &result, 1, 1000);
    while (HAL_SPI_GetState(m_spi) != HAL_SPI_STATE_READY);
    if (res != HAL_OK)
    {
        /// @todo Report an error
        return 0;
    }
    return result;
}

void NRF24L01IODriverHal::transmit(uint8_t* data, size_t size)
{
    HAL_StatusTypeDef res = HAL_SPI_Transmit(m_spi, data, size, 1000);
    while (HAL_SPI_GetState(m_spi) != HAL_SPI_STATE_READY);
    if (res != HAL_OK)
    {
        /// @todo Report an error
        return;
    }
    return;
}

void NRF24L01IODriverHal::receive(uint8_t* data, size_t size)
{
    HAL_StatusTypeDef res = HAL_SPI_Receive(m_spi, data, size, 1000);
    while (HAL_SPI_GetState(m_spi) != HAL_SPI_STATE_READY);
    if (res != HAL_OK)
    {
        /// @todo Report an error
        return;
    }
    return;
}
