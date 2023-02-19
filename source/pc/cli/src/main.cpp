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

#include <filesystem>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>

#include <thread>
#include <chrono>

std::vector<std::string> find_serial_port_urls()
{
    std::vector<std::string> result;
    for (auto const& dir_entry : std::filesystem::directory_iterator("/dev/serial/by-id"))
    {
        std::string filename = dir_entry.path().filename().u8string();
        if (filename.find("usb-WideUSB_Project_WideUSB_Virtual_ComPort") != filename.npos)
            result.push_back(dir_entry.path().u8string());
    }
    return result;
}

int main()
{
    using namespace std::chrono_literals;
    srand(time(NULL));

    std::vector<std::string> device_urls = find_serial_port_urls();
    if (device_urls.empty())
    {
        std::cout << "No wideusb devices connected to PC" << std::endl;
        return 0;
    }

    std::cout << "Devices found:" << std::endl;
    for (const auto& file : device_urls)
    {
        std::cout << file << std::endl;
    }
    std::cout << "First od them will be used: " << device_urls[0] << std::endl;

    auto runner = IOServiceRunner::create();
//    auto sp = std::make_shared<PhysicalLayerSerialPort>(runner, "/dev/ttyACM0");
    auto sp = std::make_shared<PhysicalLayerSerialPort>(runner, device_urls[0]);
//    auto sp = std::make_shared<PhysicalLayerSerialPort>(runner, "/dev/serial/by-id/usb-WideUSB_Project_WideUSB_Virtual_ComPort_316F365B3432-if00");
    auto interface = std::make_shared<NetworkInterface>(sp, std::make_shared<ChannelLayerBinary>(), false);


    auto net_srv = NetService::create(std::move(NetServiceRunnerAsio::create(runner)),
                       std::make_shared<MutexQueueFactory>(),
                       std::make_shared<NetworkLayerBinary>(),
                       std::make_shared<TransportLayerBinary>(),
                       std::make_shared<PackageInspector>(PackageInspector::Verbosity::laconic));
//                       nullptr);

    net_srv->add_interface(interface);

    DeviceDiscovery discovery(net_srv, 123);
    discovery.run();
    std::this_thread::sleep_for(5s);

    if (discovery.devices().empty())
    {
        std::cout << "Cannot discovery any device, exiting" << std::endl;
        runner->stop();
        return 0;
    }
    Address target_addr = discovery.devices().back();

    std::cout << "Creating monitor" << std::endl;
    Waiter<bool> waiter;
    auto mon = std::make_shared<MonitorFront>(net_srv, waiter.receiver(), 123, target_addr);
    std::cout << "waiting monitor" << std::endl;
    waiter.wait(10000s);
    std::cout << "monitor created" << std::endl;


    Waiter<const std::string&> waiter_status;
    std::cout << "getting status" << std::endl;
    mon->get_status_async(waiter_status.receiver());
    std::cout << waiter_status.wait(5000s) << std::endl;
    runner->stop();
}
