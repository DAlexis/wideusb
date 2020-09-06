#include "cpp-bootstrap.h"
#include "os/cpp-freertos.hpp"
#include "host-communication.hpp"
#include "modules/gps.hpp"
#include "modules/gps/nmea-receiver.hpp"
#include "macro.hpp"

#include "usart.h"
#include <usbd_cdc_if.h>
#include <stdio.h>

HostCommunicator communicator;
std::unique_ptr<CoreModule> core;
std::unique_ptr<GPSModule> gps;

void cpp_bootstrap()
{
    // here is an entry point for C++ code
    os::delay(1000);
    printf("Starting\r\n");
    gps.reset(new GPSModule);
    core.reset(new CoreModule);

    communicator.add_module(core.get());
    communicator.add_module(gps.get());
    communicator.run_thread();
    //NMEAReceiver *r = new NMEAReceiver(&huart3);
    gps->enable();
    for (;;)
    {
        os::delay(1000);
        //printf("Alive 2\r\n");
        core->assert_text("core module debug message");
    }

}
