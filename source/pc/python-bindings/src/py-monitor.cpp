#include "py-monitor.hpp"
#include "wideusb/front/monitor-front.hpp"
#include "wideusb-pc/asio-utils.hpp"

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <stdexcept>

namespace py = pybind11;

class MonitorFront;
class NetService;

class PyMonitor
{
public:
    PyMonitor(NetService& net_service, Address local_address, Address remote_address);

    std::string status();

private:

    std::unique_ptr<MonitorFront> m_monitor;
};


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

void add_monitor(pybind11::module& m)
{
    py::class_<PyMonitor>(m, "Monitor")
        .def(py::init<NetService&, Address, Address>(),
             py::arg("net_service"),
             py::arg("local_address") = 0,
             py::arg("remote_address") = 0)
        .def("status", &PyMonitor::status);
}
