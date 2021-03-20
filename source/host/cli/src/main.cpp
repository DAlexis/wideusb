#include "wideusb.hpp"
#include "host-modules/monitor.hpp"

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>

#include <thread>
#include <chrono>

using namespace WideUSBHost;
using namespace WideUSBHost::modules;

std::unique_ptr<Device> device;
std::unique_ptr<Monitor> monitor;

void on_device_created();
void on_monitor_created(bool success);
void on_status_received(const std::string& status);

void on_device_created()
{
    std::cout << "Device discovered, addr = " << device->device_address() << std::endl;
    std::cout << "Creating monitor" << std::endl;
    monitor.reset(new Monitor(*device, on_monitor_created));
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

int main()
{
    using namespace std::chrono_literals;
    srand(time(NULL));
    auto start = std::chrono::high_resolution_clock::now();

    std::unique_ptr<Monitor> monitor;
    device.reset(new Device(0x87654321, "/dev/ttyACM0", on_device_created));

    DeferredTask::run(device->io_service(), 1000, [](){ std::cout << "Deferred task 1" << std::endl; });

    DeferredTask::run(device->io_service(), 2000, [](){ std::cout << "Deferred task 2" << std::endl; });

    device->run_io_service();
    return 0;
}
