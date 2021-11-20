#include "wideusb/wideusb.hpp"
#include "wideusb-common/front/gps-front.hpp"
#include "wideusb-common/front/monitor-front.hpp"
#include "wideusb-common/front/discovery.hpp"
#include "wideusb/asio-utils.hpp"

#include "wideusb-common/communication/binary/channel.hpp"
#include "wideusb-common/communication/binary/network.hpp"
#include "wideusb-common/communication/binary/transport.hpp"


#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>

#include <thread>
#include <chrono>
/*
using namespace WideUSBHost;

std::unique_ptr<Device> device;
std::unique_ptr<MonitorFront> monitor;

void on_device_created();
void on_monitor_created(bool success);
void on_status_received(const std::string& status);

void on_device_created()
{
    std::cout << "Device discovered, addr = " << device->device_address() << std::endl;
    std::cout << "Creating monitor" << std::endl;
    monitor.reset(new MonitorFront(device->net_service(), on_monitor_created, device->host_address(), device->device_address()));
}

void on_monitor_created(bool success)
{
    std::cout << "Monitor created! success == " << success << std::endl;
    if (success)
    {
        monitor->get_status_async(on_status_received);
    }
}

void on_status_received(const std::string& status)
{
    std::cout << status << std::endl;
    DeferredTask::run(device->io_service(), 1000, [](){ monitor->get_status_async(on_status_received); });
}

void test_synchronizer()
{
    boost::asio::io_service io_service;
};
*/
int main()
{
    using namespace std::chrono_literals;
    srand(time(NULL));

    IOServiceRunner runner;
    std::shared_ptr<PhysicalLayerSerialPort> sp = std::make_shared<PhysicalLayerSerialPort>(runner.io_service(), "/dev/ttyACM0");
    NetService net_srv(sp,
                       std::make_shared<ChannelLayerBinary>(),
                       std::make_shared<NetworkLayerBinary>(),
                       std::make_shared<TransportLayerBinary>(),
                       nullptr,
                       [sp](){ sp->post_serve_sockets(); });

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
