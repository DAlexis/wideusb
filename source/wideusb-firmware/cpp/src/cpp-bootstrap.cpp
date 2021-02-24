#include "cpp-bootstrap.h"
#include "os/cpp-freertos.hpp"
#include "host-communication.hpp"
#include "modules/gps.hpp"
#include "modules/monitor.hpp"
#include "modules/gps/nmea-receiver.hpp"
#include "macro.hpp"

#include "communication/networking.hpp"
#include "host-communication/usb-physical-layer.hpp"
#include "communication/binary/channel.hpp"
#include "communication/binary/network.hpp"
#include "communication/binary/transport.hpp"

#include "usart.h"
#include <usbd_cdc_if.h>
#include <stdio.h>

//HostCommunicator communicator;

/*class CoreModule;
std::unique_ptr<CoreModule> core;
std::unique_ptr<GPSModule> gps;*/

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
    NetSevice net_srv(
        std::make_shared<USBPhysicalLayer>(),
        std::make_shared<ChannelLayerBinary>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>()
    );
    printf("net_srv created\r\n");

    MonitorModule monitor(net_srv, 0x12345678, 0x87654321, 1);
/*
    Socket sock(net_srv, 0x12345678, 0x87654321, 10);
    sock.options().retransmitting_options.cycles_count = 0;
    sock.options().retransmitting_options.timeout = 0;

    const char text[] = "hello, networking!! Im alive and sending you this data!";
    sock.send(Buffer::create(sizeof(text), text));
    printf("send\r\n");*/
    int c = 0;
    for (;;)
    {
        net_srv.serve_sockets(os::get_os_time());
        os::delay(10);
        if (c == 10)
        {
            printf("Report\r\n");
            monitor.make_report();
            c = 0;
        }
        c++;
    }

    /*gps.reset(new GPSModule);
    core.reset(new CoreModule);*/
/*
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
    }*/

}
