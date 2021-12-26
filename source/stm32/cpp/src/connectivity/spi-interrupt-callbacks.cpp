#include "connectivity/spi-interrupt-callbacks.hpp"

namespace SPI {

Interrupts spi1_interrupts;

}

extern "C" {

    void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi == &hspi1)
        {
            if (SPI::spi1_interrupts.HAL_SPI_TxCpltCallback)
                SPI::spi1_interrupts.HAL_SPI_TxCpltCallback(hspi);
        }
    }

    void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi == &hspi1)
        {
            if (SPI::spi1_interrupts.HAL_SPI_RxCpltCallback)
                SPI::spi1_interrupts.HAL_SPI_RxCpltCallback(hspi);
        }
    }

    void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi == &hspi1)
        {
            if (SPI::spi1_interrupts.HAL_SPI_TxRxCpltCallback)
                SPI::spi1_interrupts.HAL_SPI_TxRxCpltCallback(hspi);
        }
    }

    void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi == &hspi1)
        {
            if (SPI::spi1_interrupts.HAL_SPI_TxHalfCpltCallback)
                SPI::spi1_interrupts.HAL_SPI_TxHalfCpltCallback(hspi);
        }
    }

    void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi == &hspi1)
        {
            if (SPI::spi1_interrupts.HAL_SPI_RxHalfCpltCallback)
                SPI::spi1_interrupts.HAL_SPI_RxHalfCpltCallback(hspi);
        }
    }

    void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi == &hspi1)
        {
            if (SPI::spi1_interrupts.HAL_SPI_TxRxHalfCpltCallback)
                SPI::spi1_interrupts.HAL_SPI_TxRxHalfCpltCallback(hspi);
        }
    }

    void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi == &hspi1)
        {
            if (SPI::spi1_interrupts.HAL_SPI_ErrorCallback)
                SPI::spi1_interrupts.HAL_SPI_ErrorCallback(hspi);
        }
    }

    void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi == &hspi1)
        {
            if (SPI::spi1_interrupts.HAL_SPI_AbortCpltCallback)
                SPI::spi1_interrupts.HAL_SPI_AbortCpltCallback(hspi);
        }
    }

}
