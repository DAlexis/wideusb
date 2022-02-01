#include "wideusb/front/monitor-front.hpp"
#include "wideusb/communication/modules/monitor.hpp"

#include <string>

MonitorFront::MonitorFront(NetService::ptr host_connection_service, OnModuleCreatedCallbackEntry on_created, Address my_address, Address device_address) :
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
        [this](ISocketUserSide&) { socket_listener_status(); }
    ),
    m_stdout_socket(
        m_host_connection_service,
        my_address,
        ports::monitor::stdout_data,
        [this](ISocketUserSide&) { socket_listener_stdout(); }
    )
{
}

void MonitorFront::get_status_async(StatusReceivedCallbackEntry receiver)
{
    m_status_receiver = receiver;
    request_status();
}

void MonitorFront::connect_to_stdout(ConnectedToStdoutCallbackEntry on_connected, OnStdoutDataReceivedCallbackEntry on_data_received)
{
    m_on_connected_to_stdout = on_connected;
    m_on_data_received = on_data_received;
    monitor::stdout_data::SubscribeRequest request;
    request.action = monitor::stdout_data::SubscribeRequest::subscribe;

    PBuffer body = Buffer::serialize(request);
    m_stdout_socket.send(m_device_address, body);
}

void MonitorFront::socket_listener_status()
{
    ISocketUserSide::IncomingMessage incoming = *m_status_socket.get_incoming();

    monitor::status::Response response;

    if (incoming.data->size() != sizeof(response))
        return;

    BufferAccessor(incoming.data) >> response;

    std::string result = "= Monitor message from device " + std::to_string(incoming.sender) + " =\n";
    result += "total heap: " + std::to_string(response.heap_total) + "; used: " + std::to_string(response.heap_used);

    m_status_receiver.call(result);
}

void MonitorFront::socket_listener_stdout()
{
    ISocketUserSide::IncomingMessage incoming = *m_stdout_socket.get_incoming();

    if (!m_on_data_received.avaliable())
        return;

    std::string incoming_str;
    for (size_t i = 0; i < incoming.data->size(); i++)
    {
        uint8_t sym = incoming.data->at(i);
        if (sym != 0)
            incoming_str += char(sym);
    }

    m_on_data_received.call(incoming_str);
}

void MonitorFront::request_status()
{
    monitor::status::Request request;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_status_socket.send(m_device_address, body);
}
