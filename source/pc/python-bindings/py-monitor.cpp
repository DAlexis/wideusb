#include "py-monitor.hpp"

#include "pybind11/pybind11.h"

namespace py = pybind11;

#include <stdexcept>

PyMonitor::PyMonitor(NetService& net_service, Address local_address, Address remote_address)
{
    Waiter<bool> waiter;
    m_monitor.reset(
                new MonitorFront(
                    net_service, waiter.get_waiter_callback(),
                    local_address,
                    remote_address
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
