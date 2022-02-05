#include "wideusb/back/monitor-back.hpp"
#include "wideusb/communication/modules/monitor.hpp"

MonitorBack::MonitorBack(NetService::ptr net_service, Address module_address) :
    m_status_socket(net_service, module_address, ports::monitor::status_update, [this](ISocketUserSide&) { socket_listener_status(); }),
    m_stdout_socket(net_service, module_address, ports::monitor::stdout_data, [this](ISocketUserSide&) { socket_listener_stdout(); })
{
    m_status_socket.options().need_acknoledgement = false;
    m_status_socket.options().retransmitting_options.cycles_count = 1;
    m_status_socket.options().retransmitting_options.timeout = 0ms;

    m_stdout_socket.options().need_acknoledgement = false;
    m_stdout_socket.options().retransmitting_options.cycles_count = 1;
    m_stdout_socket.options().retransmitting_options.timeout = 0ms;
}


void MonitorBack::send_stdout(PBuffer buf)
{
    for (auto addr : m_subscribers)
    {
        m_stdout_socket.send(addr, buf);
    }
}

bool MonitorBack::has_stdout_subscriber()
{
    return !m_subscribers.empty();
}

void MonitorBack::socket_listener_status()
{
    monitor::status::Response response;
    MonitorStats ms = get_stats();
    response.heap_total = ms.heap_total;
    response.heap_used = ms.heap_used;
    response.allocated = ms.allocated;
    response.malloc_times = ms.malloc_times;
    response.malloc_isr_times = ms.malloc_isr_times;
    response.free_times = ms.free_times;
    response.free_isr_times = ms.free_isr_times;

    PBuffer resp_buffer = Buffer::create(sizeof(response), &response);

    while (m_status_socket.has_incoming())
    {
        Socket::IncomingMessage incoming = *m_status_socket.get_incoming();

        auto request = try_interpret_buffer_no_magic<monitor::status::Request>(incoming.data);
        if (!request)
            continue;

        m_status_socket.send(incoming.sender, resp_buffer);
    }
}

void MonitorBack::socket_listener_stdout()
{
    while (m_stdout_socket.has_incoming())
    {
        Socket::IncomingMessage incoming = *m_stdout_socket.get_incoming();
        auto request = try_interpret_buffer_no_magic<monitor::stdout_data::SubscribeRequest>(incoming.data);
        if (!request)
            continue;
        if (request->action == monitor::stdout_data::SubscribeRequest::subscribe)
            m_subscribers.push_back(incoming.sender);
    }
}
