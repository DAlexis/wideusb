#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "communication/networking.hpp"

#include <string>
#include <optional>

class WideUSBDevice;

class Monitor
{
public:
    Monitor(WideUSBDevice& host_connection_service, Address device_addr, Address host_address, Port port);

    std::optional<std::string> get_status();
    void request_status();

private:
    WideUSBDevice& m_host_connection_service;

    Address m_device_addr;
    Socket m_sock;
};

#endif // MONITOR_HPP
