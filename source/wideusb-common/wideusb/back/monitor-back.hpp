#ifndef MONITORBACK_HPP
#define MONITORBACK_HPP

#include "wideusb/communication/networking.hpp"
#include "wideusb/back/base-back.hpp"

class MonitorBack : public IModule
{
public:
    MonitorBack(NetService& net_service, Address module_address);

protected:
    Socket m_sock;
    virtual uint32_t get_heap_used() = 0;
    virtual uint32_t get_heap_total() = 0;

private:
    void socket_listener();


};

#endif // MONITORBACK_HPP
