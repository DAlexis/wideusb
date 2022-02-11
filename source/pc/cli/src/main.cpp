#include "wideusb-pc/asio-utils.hpp"
#include "wideusb-pc/physical-layer-serial-port.hpp"
#include "wideusb-pc/socket-queue-mutex.hpp"
#include "wideusb-pc/asio-net-srv-runner.hpp"
#include "wideusb-pc/package-inspector.hpp"
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
    auto sp = std::make_shared<PhysicalLayerSerialPort>(runner, "/dev/ttyACM0");
    auto interface = std::make_shared<NetworkInterface>(sp, std::make_shared<ChannelLayerBinary>(), true);


    auto net_srv = NetService::create(std::move(NetServiceRunnerAsio::create(runner)),
                       std::make_shared<MutexQueueFactory>(),
                       std::make_shared<NetworkLayerBinary>(),
                       std::make_shared<TransportLayerBinary>(),
//                       std::make_shared<PackageInspector>());
                       nullptr);

    net_srv->add_interface(interface);

    DeviceDiscovery discovery(net_srv, 123);
    discovery.run();
    std::this_thread::sleep_for(1s);

    if (discovery.devices().empty())
    {
        std::cout << "Cannot discovery any device, exiting" << std::endl;
        runner->stop();
        return 0;
    }
    Address target_addr = discovery.devices().front();

    std::cout << "Creating monitor" << std::endl;
    Waiter<bool> waiter;
    auto mon = std::make_shared<MonitorFront>(net_srv, waiter.receiver(), 123, target_addr);
    std::cout << "waiting monitor" << std::endl;
    waiter.wait(1s);


    Waiter<const std::string&> waiter_status;
    mon->get_status_async(waiter_status.receiver());
    std::cout << waiter_status.wait(5s) << std::endl;
    runner->stop();
}
