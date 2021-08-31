#include "wideusb-common/front/monitor-front.hpp"
#include "communication/modules/monitor.hpp"

#include <string>

MonitorFront::MonitorFront(NetSevice& host_connection_service, OnModuleCreatedCallback on_created, Address my_address, Address device_address) :
    ModuleFrontBase(
        host_connection_service, monitor::id,
        device_address,
        my_address,
        on_created
    ),
    m_status_socket(
        m_host_connection_service,
        my_address,
        ports::monitor::status_update,
        [this](ISocketUserSide&) { socket_listener(); }
    )
{
}

void MonitorFront::get_status_async(StatusReceivedCallback callback)
{
    m_on_status_updated = callback;
    request_status();
}

void MonitorFront::socket_listener()
{
    ISocketUserSide::IncomingMessage incoming = *m_status_socket.get();

    monitor::status::Response response;

    if (incoming.data->size() != sizeof(response))
        return;

    BufferAccessor(incoming.data) >> response;

    std::string result = "= Monitor message from device " + std::to_string(incoming.sender) + " =\n";
    result += "total heap: " + std::to_string(response.heap_total) + "; used: " + std::to_string(response.heap_used);

    if (m_on_status_updated)
        m_on_status_updated(result);
}

void MonitorFront::request_status()
{
    monitor::status::Request request;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_status_socket.send(m_device_address, body);
}
