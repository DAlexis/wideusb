#include "monitor.hpp"

Monitor::Monitor(NetSevice& net_service, Address device_addr, Address host_address, Port port) :
    m_device_addr(device_addr),
    m_sock(net_service, host_address, port)
{
    m_sock.options().input_queue_limit = 1; // We need only last status message
    m_sock.options().need_acknoledgement = false;
    m_sock.options().retransmitting_options.cycles_count = 1;
    m_sock.options().retransmitting_options.timeout = 0;
}

std::optional<std::string> Monitor::get_status()
{
    if (!m_sock.has_data())
        return std::nullopt;

    std::optional<ISocketUserSide::IncomingMessage> msg = m_sock.get();

    std::string result = "= Monitor message from device " + std::to_string(msg->sender) + " =\n";
    result += (const char*)msg->data->data();
    return result;
}

void Monitor::request_status()
{
    PBuffer body = Buffer::create(1, "X");
    m_sock.send(m_device_addr, body);
}
