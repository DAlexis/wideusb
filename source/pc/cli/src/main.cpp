#include "wideusb-pc/asio-utils.hpp"
#include "wideusb-pc/physical-layer-serial-port.hpp"
#include "wideusb-pc/socket-queue-mutex.hpp"
#include "wideusb/front/gps-front.hpp"
#include "wideusb/front/monitor-front.hpp"
#include "wideusb/front/discovery.hpp"


#include "wideusb/communication/binary/channel.hpp"
#include "wideusb/communication/binary/network.hpp"
#include "wideusb/communication/binary/transport.hpp"


#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>

#include <thread>
#include <chrono>

int main()
{
    using namespace std::chrono_literals;
    srand(time(NULL));

    auto runner = IOServiceRunner::create();
    auto sp = std::make_shared<PhysicalLayerSerialPort>(runner->io_service(), "/dev/ttyACM0");
    auto interface = std::make_shared<NetworkInterface>(sp, std::make_shared<ChannelLayerBinary>(), true);


    NetService net_srv(std::make_shared<MutexQueueFactory>(),
                       std::make_shared<NetworkLayerBinary>(),
                       std::make_shared<TransportLayerBinary>(),
                       [sp](){ sp->post_serve_sockets(); });

    net_srv.add_interface(interface);

    DeviceDiscovery discovery(net_srv, 123);
    discovery.run();
    std::this_thread::sleep_for(5s);

    /*

    srand(time(NULL));

    device.reset(new Device(0x87654321, "/dev/ttyACM0", on_device_created));

    DeferredTask::run(device->io_service(), 1000, [](){ std::cout << "Deferred task 1" << std::endl; });

    DeferredTask::run(device->io_service(), 2000, [](){ std::cout << "Deferred task 2" << std::endl; });

    AsioServiceRunner runner(device->io_service());
    runner.run_thread();
    std::this_thread::sleep_for(10s);

    GPSFront gps(device->net_service(), nullptr, device->host_address(), device->device_address());
    //device->run_io_service();
    return 0;*/
}
