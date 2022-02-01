#include "py-monitor.hpp"
#include "wideusb/front/monitor-front.hpp"
#include "wideusb-pc/asio-utils.hpp"

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <stdexcept>
#include <string>

namespace py = pybind11;

class MonitorFront;
class NetService;

class PyMonitor
{
public:
    PyMonitor(NetService::ptr net_service, Address local_address, Address remote_address);

    std::string status();
    void enable_stdout_print();

private:

    std::unique_ptr<MonitorFront> m_monitor;
    std::shared_ptr<CallbackReceiver<const std::string&>> m_stdout_callback_receiver =
            CallbackReceiver<const std::string&>::create( [](const std::string& text) { std::cout << text; } );
};


PyMonitor::PyMonitor(NetService::ptr net_service, Address local_address, Address remote_address)
{
    Waiter<bool> waiter;
    m_monitor.reset(
                new MonitorFront(
                    net_service, waiter.receiver(),
                    local_address,
                    remote_address
                    )
                );
    bool success = waiter.wait(1s);
    if (!success)
        throw std::runtime_error("Monitor module creation failed");
}

std::string PyMonitor::status()
{
    Waiter<const std::string&> waiter;
    m_monitor->get_status_async(waiter.receiver());
    std::string result = waiter.wait(5s);
    return result;
}

void PyMonitor::enable_stdout_print()
{
//    Waiter<bool> waiter;
    m_monitor->connect_to_stdout(nullptr, m_stdout_callback_receiver);
//    bool success = waiter.wait();
//    if (!success)
//        throw std::runtime_error("Monitor module creation failed");
}

void add_monitor(pybind11::module& m)
{
    py::class_<PyMonitor>(m, "Monitor")
        .def(py::init<NetService::ptr, Address, Address>(),
             py::arg("net_service"),
             py::arg("local_address") = 0,
             py::arg("remote_address") = 0)
        .def("status", &PyMonitor::status)
        .def("enable_stdout_print", &PyMonitor::enable_stdout_print);
}
