#include "py-monitor.hpp"

#include "pybind11/pybind11.h"

namespace py = pybind11;

#include <stdexcept>

PyMonitor::PyMonitor(PyWideUSBDevice& device, Address custom_host_address, Address custom_device_address)
{
    Waiter<bool> waiter;
    m_monitor.reset(
                new MonitorFront(
                    device.device().net_service(), waiter.get_waiter_callback(),
                    custom_host_address ? custom_host_address : device.device().host_address(),
                    custom_device_address ? custom_device_address : device.device().device_address()
                    )
                );
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
