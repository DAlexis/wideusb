#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "communication/networking.hpp"
#include "host-module.hpp"

#include <string>
#include <optional>

namespace WideUSBHost
{

class Device;

namespace modules
{

class Monitor : public ModuleBase
{
public:
    using StatusReceivedCallback = std::function<void(const std::string&)>;
    Monitor(Device& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address = 0, Address custom_device_address = 0);

    void get_status_async(StatusReceivedCallback callback);

private:
    void request_status();
    void socket_listener();

    Socket m_status_socket;
    StatusReceivedCallback m_on_status_updated;
};

}

}

#endif // MONITOR_HPP
