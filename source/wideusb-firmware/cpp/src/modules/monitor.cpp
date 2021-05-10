#include "modules/monitor.hpp"
#include "communication/modules/monitor.hpp"
#include "communication/modules/ports.hpp"

#include "newlib-monitor.h"

#include <string>

MonitorModule::MonitorModule(NetSevice& net_service, Address monitor_address) :
    m_sock(net_service, monitor_address, ports::monitor::status_update, [this](ISocketUserSide&) { socket_listener(); })
{
    m_sock.options().need_acknoledgement = false;
    m_sock.options().retransmitting_options.cycles_count = 1;
    m_sock.options().retransmitting_options.timeout = 0;
}

void MonitorModule::tick()
{

}

void MonitorModule::socket_listener()
{
    monitor::status::Response response;
    response.heap_total = heap_total;
    response.heap_used = heap_used;
    PBuffer resp_buffer = Buffer::create(sizeof(response), &response);

    while (m_sock.has_data())
    {
        Socket::IncomingMessage incoming = *m_sock.get();

        auto request = try_interpret_buffer_no_magic<monitor::status::Request>(incoming.data);
        if (!request)
            continue;

        m_sock.send(incoming.sender, resp_buffer);
    }
}
