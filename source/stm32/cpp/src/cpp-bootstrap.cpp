#include "cpp-bootstrap.h"
#include "os/cpp-freertos.hpp"
#include "wideusb-device.hpp"

#include "usart.h"
#include <usbd_cdc_if.h>
#include <stdio.h>

#include "dac.h"
#include "dma.h"
#include "tim.h"

#include "devices/nrf24l01-driver-impl.hpp"
#include "devices/nrf24l01.hpp"

#include <chrono>


void cpp_bootstrap()
{
    // here is an entry point for C++ code
    os::delay(2000ms);

    /*
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    fill_dac_buffer();
    HAL_TIM_Base_Start(&htim4);
    run_dac();
    */

    printf("Starting\n");

    auto driver = NRF24L01IODriverImpl::get();
    printf("Driver created\n");
    NRF24L01Manager nrf(driver);
    printf("NRF24L01 device created\n");
    nrf.printStatus();
    printf("Status should be above\n");

    {
        int secs = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()
                    ).count();
        printf("msecs = %d\n", secs);
    }

    WideusbDevice dev;
    dev.run();

}
