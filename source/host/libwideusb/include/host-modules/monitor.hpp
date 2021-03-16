#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "communication/networking.hpp"
#include "host-module.hpp"

#include <string>
#include <optional>

class WideUSBDevice;

class Monitor : public ModuleOnHostBase
{
public:
    using StatusReceivedCallback = std::function<void(const std::string&)>;
    Monitor(WideUSBDevice& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address = 0, Address custom_device_address = 0);

    void on_device_ready() override;

    void get_status_async(StatusReceivedCallback callback);

    void request_status();

private:
    void on_incoming_data_callback();

    Socket m_status_socket;
    StatusReceivedCallback m_on_status_updated;
};

#endif // MONITOR_HPP
