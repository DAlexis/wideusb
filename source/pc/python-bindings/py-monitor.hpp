#ifndef PYMONITOR_HPP
#define PYMONITOR_HPP

#include "wideusb/front/monitor-front.hpp"
#include "py-wideusb-device.hpp"

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

#endif // PYMONITOR_HPP
