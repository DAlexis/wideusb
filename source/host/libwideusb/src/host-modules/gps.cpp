#include "host-modules/gps.hpp"
#include "communication/modules/gps.hpp"
#include "wideusb.hpp"


using namespace WideUSBHost::modules;

GPS::GPS(Device& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address, Address custom_device_address) :
    ModuleBase(
        host_connection_service, gps::id,
        custom_device_address != 0 ? custom_device_address : host_connection_service.device_address(),
        custom_host_address != 0 ? custom_host_address : host_connection_service.host_address(),
        on_created
    ),
    m_position_socket(
        m_host_connection_service.net_service(),
        custom_host_address != 0 ? custom_host_address : m_host_connection_service.host_address(),
        ports::gps::position_update,
        [this](ISocketUserSide&) { socket_listener(); }
    )
{
}

void GPS::get_position_async(PositionReceivedCallback callback)
{
    m_on_status_updated = callback;
    request_status();
}

void GPS::request_status()
{
    gps::positioning::Request request;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_position_socket.send(m_device_address, body);
}

void GPS::socket_listener()
{
    ISocketUserSide::IncomingMessage incoming = *m_position_socket.get();

    gps::positioning::Response response;

    if (incoming.data->size() != sizeof(response))
        return;

    BufferAccessor(incoming.data) >> response;

    Position result;
    result.latitude = response.latitude;
    result.longitude = response.longitude;
    result.has_pps = response.has_pps;

    if (m_on_status_updated)
        m_on_status_updated(result);
}
