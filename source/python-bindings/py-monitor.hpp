#ifndef PYMONITOR_HPP
#define PYMONITOR_HPP

#include "wideusb-common/front/monitor-front.hpp"
#include "py-wideusb-device.hpp"

class MonitorFront;

class PyMonitor
{
public:
    PyMonitor(PyWideUSBDevice& device, Address custom_host_address = 0, Address custom_device_address = 0);

    std::string status();

private:

    std::unique_ptr<MonitorFront> m_monitor;
};

#endif // PYMONITOR_HPP
