#include "cpp-bootstrap.h"
#include "os/cpp-freertos.hpp"
#include "wideusb-device.hpp"

#include <usbd_cdc_if.h>
#include <stdio.h>

void cpp_bootstrap()
{
    // here is an entry point for C++ code
    printf("Waiting 2s...\r\n");
    os::delay(2000ms);

    printf("Starting\r\n");

    WideusbDevice dev;
    dev.run();

}
