#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "communication/networking.hpp"

#include <string>
#include <optional>

class Monitor
{
public:
    Monitor(NetSevice& net_service, Address monitor_address, Address destination_address, Port port);

    std::optional<std::string> get_status();

private:

    Socket m_sock;
};

#endif // MONITOR_HPP
