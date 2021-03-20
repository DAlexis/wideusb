#ifndef PYMONITOR_HPP
#define PYMONITOR_HPP

#include "host-modules/monitor.hpp"
#include "py-wideusb-device.hpp"

class PyMonitor
{
public:
    PyMonitor(PyWideUSBDevice& device, Address custom_host_address = 0, Address custom_device_address = 0);

    std::string status();

private:

    std::unique_ptr<WideUSBHost::modules::Monitor> m_monitor;
};

#endif // PYMONITOR_HPP
