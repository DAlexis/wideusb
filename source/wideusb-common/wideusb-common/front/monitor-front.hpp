#ifndef MONITORFRONT_HPP
#define MONITORFRONT_HPP

#include "wideusb-common/front/base.hpp"
#include "communication/networking.hpp"

class MonitorFront : public ModuleFrontBase
{
public:
    using StatusReceivedCallback = std::function<void(const std::string&)>;
    MonitorFront(NetSevice& host_connection_service, OnModuleCreatedCallback on_created, Address my_address, Address device_address);
    void get_status_async(StatusReceivedCallback callback);

private:
    void request_status();
    void socket_listener();

    Socket m_status_socket;
    StatusReceivedCallback m_on_status_updated;
};



#endif // MONITORFRONT_HPP
