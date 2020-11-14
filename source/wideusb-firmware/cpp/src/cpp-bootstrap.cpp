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
    os::delay(1000);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    fill_dac_buffer();
    HAL_TIM_Base_Start(&htim4);
    run_dac();

    printf("Starting\r\n");
    gps.reset(new GPSModule);
    core.reset(new CoreModule);

    communicator.add_module(core.get());
    communicator.set_core_module(core.get());

    communicator.add_module(gps.get());
    communicator.run_thread();
    //NMEAReceiver *r = new NMEAReceiver(&huart3);
    gps->enable();
    for (;;)
    {
        os::delay(1000);
        //printf("Alive 2\r\n");
        core->assert_text("cpp_bootstrap() main cycle alive");
    }

}
