#ifndef MONITORBACK_HPP
#define MONITORBACK_HPP

#include "communication/networking.hpp"

class MonitorBack
{
public:
    MonitorBack(NetSevice& net_service, Address module_address);

protected:
    Socket m_sock;
    virtual uint32_t get_heap_used() = 0;
    virtual uint32_t get_heap_total() = 0;

private:
    void socket_listener();


};

#endif // MONITORBACK_HPP
