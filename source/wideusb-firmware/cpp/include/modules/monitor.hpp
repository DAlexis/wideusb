#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "communication/networking.hpp"

class MonitorModule
{
public:
    MonitorModule(NetSevice& net_service, Address monitor_address, Port port);

    void make_report();

private:

    Socket m_sock;

};

#endif // MONITOR_HPP
