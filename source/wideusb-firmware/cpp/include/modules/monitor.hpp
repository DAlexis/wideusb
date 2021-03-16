#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "communication/networking.hpp"
#include "module-base.hpp"

class MonitorModule : public IModule
{
public:
    MonitorModule(NetSevice& net_service, Address monitor_address);

    void tick();

private:
    void socket_listener();

    Socket m_sock;

};

#endif // MONITOR_HPP
