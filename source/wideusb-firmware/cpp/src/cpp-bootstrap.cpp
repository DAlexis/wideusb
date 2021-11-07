#include "cpp-bootstrap.h"
#include "os/cpp-freertos.hpp"
#include "modules/gps-impl.hpp"
#include "modules/monitor-impl.hpp"
#include "modules/gps/nmea-receiver.hpp"
#include "modules/core.hpp"
#include "macro.hpp"

#include "wideusb-common/communication/networking.hpp"
#include "host-communication/usb-physical-layer.hpp"
#include "wideusb-common/communication/binary/channel.hpp"
#include "wideusb-common/communication/binary/network.hpp"
#include "wideusb-common/communication/binary/transport.hpp"

#include "wideusb-device.hpp"

#include "usart.h"
#include <usbd_cdc_if.h>
#include <stdio.h>

#include "dac.h"
#include "dma.h"
#include "tim.h"

extern "C" {
void run_dac();
void fill_dac_buffer();
}

void cpp_bootstrap()
{
    // here is an entry point for C++ code
    os::delay(2000);

    /*
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    fill_dac_buffer();
    HAL_TIM_Base_Start(&htim4);
    run_dac();
    */

    printf("Starting\r\n");
    WideusbDevice dev;
    dev.run();

}
