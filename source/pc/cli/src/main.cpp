#include "wideusb-pc/asio-utils.hpp"
#include "wideusb-pc/physical-layer-serial-port.hpp"
#include "wideusb-pc/socket-queue-mutex.hpp"
#include "wideusb-pc/asio-net-srv-runner.hpp"
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


    NetService net_srv(std::move(NetServiceRunnerAsio::create(runner)),
                       std::make_shared<MutexQueueFactory>(),
                       std::make_shared<NetworkLayerBinary>(),
                       std::make_shared<TransportLayerBinary>());

    net_srv.add_interface(interface);

    DeviceDiscovery discovery(net_srv, 123);
    discovery.run();
    std::this_thread::sleep_for(5s);
}
