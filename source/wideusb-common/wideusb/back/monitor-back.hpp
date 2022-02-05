#ifndef MONITORBACK_HPP
#define MONITORBACK_HPP

#include "wideusb/communication/networking.hpp"
#include "wideusb/back/base-back.hpp"
#include "wideusb/modules/monitor-types.hpp"

#include <list>

class MonitorBack : public IModule
{
public:
    MonitorBack(NetService::ptr net_service, Address module_address);

protected:
    Socket m_status_socket;
    Socket m_stdout_socket;

    virtual MonitorStats get_stats() = 0;

    void send_stdout(PBuffer buf);
    bool has_stdout_subscriber();

private:
    void socket_listener_status();
    void socket_listener_stdout();

    std::list<Address> m_subscribers;
};

#endif // MONITORBACK_HPP
