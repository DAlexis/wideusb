#include "py-monitor.hpp"

#include "pybind11/pybind11.h"

namespace py = pybind11;

#include <stdexcept>

PyMonitor::PyMonitor(PyWideUSBDevice& device, Address custom_host_address, Address custom_device_address)
{
    std::cout << "custom_device_address " << custom_device_address << " custom_host_address " << custom_host_address << std::endl;
    Waiter<bool> waiter;
    m_monitor.reset(new WideUSBHost::modules::Monitor(device.device(), waiter.get_waiter_callback(), custom_host_address, custom_device_address));
    bool success = waiter.wait();
    if (!success)
        throw std::runtime_error("Monitor module creation failed");
}

std::string PyMonitor::status()
{
    Waiter<std::string> waiter;
    m_monitor->get_status_async(waiter.get_waiter_callback());
    std::string result = waiter.wait();
    return result;
}
