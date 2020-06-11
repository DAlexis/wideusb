#include "cpp-bootstrap.h"
#include "os/cpp-freertos.hpp"
#include "host-communication.hpp"
#include "modules/gps.hpp"
#include "modules/gps/nmea-receiver.hpp"
#include "macro.hpp"

#include "usart.h"
#include <usbd_cdc_if.h>
#include <stdio.h>


uint8_t buffer[2048];

HostCommunicator communicator;
CoreModule core;
std::unique_ptr<GPSModule> gps;

void cpp_bootstrap()
{
    //char msg[] = "Hello, im a USB device\r\n";
    char not_empty[] = "Hello, im a USB device\r\n";
    uint32_t len = 0;
    communicator.add_module(&core);
    communicator.run_thread();

    os::delay(5000);
    printf("Starting\r\n");
    //gps.reset(new GPSModule);
    NMEAReceiver *r = new NMEAReceiver(&huart3);
    for (;;)
    {
        os::delay(1000);
        //printf("Alive 2\r\n");
        //core.report_debug("core module debug message");
        //core.report_debug("core module error message");
        //printf("Input buffer ->\r\n");
//        printf(reinterpret_cast<const char*>(USBD_input_buffer.ring_buffer));
        //printf("<-\r\n");

/*
        CDC_Transmit_FS((uint8_t*) msg, ARRAY_SIZE(msg));

        if (len != 0)
        {
            CDC_Transmit_FS((uint8_t*) not_empty, ARRAY_SIZE(not_empty));
        }*/
    }
    // here is an entry point for C++ code
}
