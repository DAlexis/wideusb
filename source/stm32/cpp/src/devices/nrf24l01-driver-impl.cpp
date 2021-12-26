#include "devices/nrf24l01-driver-impl.hpp"
#include "os/cpp-freertos.hpp"
#include "connectivity/spi-interrupt-callbacks.hpp"

#include "main.h"
#include "spi.h"

#include <cstdio>

//#define DEBUG_NRF_IO_DRIVER

NRF24L01IODriverImpl::ptr NRF24L01IODriverImpl::impl;

NRF24L01IODriverImpl::ptr NRF24L01IODriverImpl::get()
{
    if (!impl)
    {
        impl = ptr(new NRF24L01IODriverImpl());
    }
    return impl;
}

NRF24L01IODriverImpl::NRF24L01IODriverImpl()
{
    auto ok_callback = [this](SPI_HandleTypeDef*) { on_tx_rx_ok_interrupt(); };
    auto error_callback = [this](SPI_HandleTypeDef*) { on_tx_rx_error_interrupt(); };

    SPI::spi1_interrupts.HAL_SPI_RxCpltCallback = ok_callback;
    SPI::spi1_interrupts.HAL_SPI_TxCpltCallback = ok_callback;
    SPI::spi1_interrupts.HAL_SPI_TxRxCpltCallback = ok_callback;
    SPI::spi1_interrupts.HAL_SPI_AbortCpltCallback = ok_callback;

    SPI::spi1_interrupts.HAL_SPI_ErrorCallback = error_callback;
}

void NRF24L01IODriverImpl::set_chip_select(bool state)
{
    HAL_GPIO_WritePin(NRF24L01_CSN_GPIO_Port, NRF24L01_CSN_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void NRF24L01IODriverImpl::set_chip_enable(bool state)
{
    HAL_GPIO_WritePin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool NRF24L01IODriverImpl::get_irq_pin()
{
    return HAL_GPIO_ReadPin(NRF24L01_IRQ_GPIO_Port, NRF24L01_IRQ_Pin) == GPIO_PIN_SET;
}

uint8_t NRF24L01IODriverImpl::transmit_receive(uint8_t byte)
{
#ifdef DEBUG_NRF_IO_DRIVER
    printf("SPI transmit_receive go\n");
#endif
    m_done = false;
    uint8_t result = 0;
    os::CriticalSection cs;
    HAL_StatusTypeDef res = HAL_SPI_TransmitReceive_IT(&hspi1, &byte, &result, 1);
    cs.unlock();
    if (res != HAL_OK)
    {
#ifdef DEBUG_NRF_IO_DRIVER
        printf("SPI transmit_receive failed, res = %d!\n", res);
#endif
        return 0;
    }

    if (!wait_for_done(io_timeout))
    {
#ifdef DEBUG_NRF_IO_DRIVER
        printf("SPI transmit_receive timeout!\n");
#endif
        return 0;
    }
#ifdef DEBUG_NRF_IO_DRIVER
    printf("    SPI transmit_receive success\n");
#endif
    return result;
}

void NRF24L01IODriverImpl::transmit(uint8_t* data, size_t size)
{
#ifdef DEBUG_NRF_IO_DRIVER
    printf("SPI transmit go\n");
#endif
    m_done = false;
    os::CriticalSection cs;
    HAL_StatusTypeDef res = HAL_SPI_Transmit_IT(&hspi1, data, size);
    cs.unlock();
    if (res != HAL_OK)
    {
#ifdef DEBUG_NRF_IO_DRIVER
        printf("SPI transmit failed, res = %d!\n", res);
#endif
        return;
    }

    if (!wait_for_done(io_timeout))
    {
#ifdef DEBUG_NRF_IO_DRIVER
        printf("SPI transmit timeout!\n");
#endif
        return;
    }
#ifdef DEBUG_NRF_IO_DRIVER
    printf("    SPI transmit success\n");
#endif
}

void NRF24L01IODriverImpl::receive(uint8_t* data, size_t size)
{
#ifdef DEBUG_NRF_IO_DRIVER
    printf("SPI receive go\n");
#endif
    m_done = false;
    os::CriticalSection cs;
    HAL_StatusTypeDef res = HAL_SPI_Receive_IT(&hspi1, data, size);
    cs.unlock();
    if (res != HAL_OK)
    {
#ifdef DEBUG_NRF_IO_DRIVER
        printf("SPI receive failed, res = %d!\n", res);
#endif
        return;
    }

    if (!wait_for_done(io_timeout))
    {
#ifdef DEBUG_NRF_IO_DRIVER
        printf("SPI receive timeout!\n");
#endif
        return;
    }
#ifdef DEBUG_NRF_IO_DRIVER
    printf("    SPI receive success\n");
#endif
}

void NRF24L01IODriverImpl::on_tx_rx_ok_interrupt()
{
    m_done = true;
}

void NRF24L01IODriverImpl::on_tx_rx_error_interrupt()
{
    m_done = true;
}

bool NRF24L01IODriverImpl::wait_for_done(int64_t timeout_ms)
{
    int64_t start_time = os::get_os_time();
    while (!m_done) {
        if (os::get_os_time() - start_time > timeout_ms)
        {
            // Not false to prevent case when context switching cause 'timeout',
            // but not real timeout
            return m_done;
        }
    }
    return m_done;
}
