#include "monitor.hpp"

Monitor::Monitor(NetSevice& net_service, Address monitor_address, Address destination_address, Port port) :
    m_sock(net_service, monitor_address, destination_address, port)
{
    m_sock.options().input_queue_limit = 1; // We need only last status message
}

std::optional<std::string> Monitor::get_status()
{
    if (m_sock.has_data())
        return std::string((const char*)m_sock.get()->data());

    return std::nullopt;
}
