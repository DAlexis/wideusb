#include "cpp-bootstrap.h"
#include "os/cpp-freertos.hpp"
#include "macro.hpp"

#include <usbd_cdc_if.h>

void cpp_bootstrap()
{
    char msg[] = "Hello, im a USB device\r\n";
    for (;;)
    {
        os::delay(1000);
        CDC_Transmit_FS((uint8_t*) msg, ARRAY_SIZE(msg));

    }
    // here is an entry point for C++ code
}
