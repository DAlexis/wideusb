#ifndef SPIINTERRUPTCALLBACKS_HPP
#define SPIINTERRUPTCALLBACKS_HPP

#include <functional>
#include "spi.h"

namespace SPI {

struct Interrupts
{
    using Callback = std::function<void(SPI_HandleTypeDef* hspi)>;

    Callback HAL_SPI_TxCpltCallback = nullptr;
    Callback HAL_SPI_RxCpltCallback = nullptr;
    Callback HAL_SPI_TxRxCpltCallback = nullptr;
    Callback HAL_SPI_TxHalfCpltCallback = nullptr;
    Callback HAL_SPI_RxHalfCpltCallback = nullptr;
    Callback HAL_SPI_TxRxHalfCpltCallback = nullptr;
    Callback HAL_SPI_ErrorCallback = nullptr;
    Callback HAL_SPI_AbortCpltCallback = nullptr;
};

extern Interrupts spi1_interrupts;

}

#endif // SPIINTERRUPTCALLBACKS_HPP
