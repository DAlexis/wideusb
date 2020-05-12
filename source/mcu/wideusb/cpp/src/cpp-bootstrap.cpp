#include "cpp-bootstrap.h"
#include "os/cpp-freertos.hpp"
#include "macro.hpp"

#include <usbd_cdc_if.h>
#include <stdio.h>

uint8_t buffer[2048];

void cpp_bootstrap()
{
    //char msg[] = "Hello, im a USB device\r\n";
    char not_empty[] = "Hello, im a USB device\r\n";
    uint32_t len = 0;
    for (;;)
    {
        os::delay(1000);
        printf("Input buffer ->\r\n");
//        printf(reinterpret_cast<const char*>(USBD_input_buffer.ring_buffer));
        printf("<-\r\n");

/*
        CDC_Transmit_FS((uint8_t*) msg, ARRAY_SIZE(msg));

        if (len != 0)
        {
            CDC_Transmit_FS((uint8_t*) not_empty, ARRAY_SIZE(not_empty));
        }*/
    }
    // here is an entry point for C++ code
}
