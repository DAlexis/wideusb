#include "host-modules/monitor.hpp"

#include "wideusb.hpp"
#include "communication/modules/ports.hpp"
#include "communication/modules/ids.hpp"
#include "communication/modules/monitor.hpp"

#include <stdexcept>
#include <iostream>

using namespace WideUSBHost;
using namespace WideUSBHost::modules;

Monitor::Monitor(Device& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address, Address custom_device_address) :
    ModuleBase(
        host_connection_service, ids::monitor,
        custom_device_address != 0 ? custom_device_address : host_connection_service.device_address(),
        custom_host_address != 0 ? custom_host_address : host_connection_service.host_address(),
        on_created
    ),
    m_status_socket(
        m_host_connection_service.net_service(),
        custom_host_address != 0 ? custom_host_address : m_host_connection_service.host_address(),
        ports::monitor::status_update,
        [this](ISocketUserSide&) { socket_listener(); }
    )
{
    m_status_socket.options().input_queue_limit = 1; // We need only last status message
    m_status_socket.options().need_acknoledgement = false;
    m_status_socket.options().retransmitting_options.cycles_count = 1;
    m_status_socket.options().retransmitting_options.timeout = 0;
}

void Monitor::get_status_async(StatusReceivedCallback callback)
{
    m_on_status_updated = callback;
    request_status();
}

void Monitor::socket_listener()
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

void Monitor::request_status()
{
    monitor::status::Request request;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_status_socket.send(m_device_address, body);
}
