#include "cpp-bootstrap.h"
#include "os/cpp-freertos.hpp"
#include "wideusb-device.hpp"

#include "usart.h"
#include <usbd_cdc_if.h>
#include <stdio.h>

#include "dac.h"
#include "dma.h"
#include "tim.h"

void cpp_bootstrap()
{
    // here is an entry point for C++ code
    os::delay(2000ms);

    printf("Starting\n");

    printf("Driver created\n");

    WideusbDevice dev;
    dev.run();

}
