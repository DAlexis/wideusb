#include "wideusb-common/back/monitor-back.hpp"
#include "wideusb-common/communication/modules/monitor.hpp"

MonitorBack::MonitorBack(NetService& net_service, Address module_address) :
    m_sock(net_service, module_address, ports::monitor::status_update, [this](ISocketUserSide&) { socket_listener(); })
{
    m_sock.options().need_acknoledgement = false;
    m_sock.options().retransmitting_options.cycles_count = 1;
    m_sock.options().retransmitting_options.timeout = 0;
}


void MonitorBack::socket_listener()
{
    monitor::status::Response response;
    response.heap_total = get_heap_total();
    response.heap_used = get_heap_used();
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
