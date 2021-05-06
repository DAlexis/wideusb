#include "host-modules/gps.hpp"
#include "communication/modules/gps.hpp"
#include "wideusb.hpp"
#include <iostream>

using namespace WideUSBHost::modules;

GPS::GPS(Device& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address, Address custom_device_address) :
    ModuleBase(
        host_connection_service, gps::id,
        custom_device_address != 0 ? custom_device_address : host_connection_service.device_address(),
        custom_host_address != 0 ? custom_host_address : host_connection_service.host_address(),
        on_created
    ),
    m_sock_position(
        m_host_connection_service.net_service(),
        custom_host_address != 0 ? custom_host_address : m_host_connection_service.host_address(),
        ports::gps::positioning,
        [this](ISocketUserSide&) { socket_listener_positioning(); }
    ),
    m_sock_timestamping(
        m_host_connection_service.net_service(),
        custom_host_address != 0 ? custom_host_address : m_host_connection_service.host_address(),
        gps::timestamping::port,
        [this](ISocketUserSide&) { socket_listener_timestamp(); }
    )
{
}

void GPS::get_position_async(PositionReceivedCallback callback)
{
    m_on_status_updated = callback;
    request_status();
}

void GPS::subscribe_to_timestamping(SubscribedCallback on_subscribe, TimestampingCallback on_timestamp)
{
    m_on_subscribed = on_subscribe;
    m_on_timestamping = on_timestamp;

    gps::timestamping::SubscribeRequest subscribe_req;
    subscribe_req.action = gps::timestamping::SubscribeRequest::add;
    subscribe_req.subscriber = m_host_address;

    std::cout << "Sizeof(subscribe_req) = " << sizeof(subscribe_req) << std::endl;

    PBuffer pb = Buffer::serialize(subscribe_req);
    auto inter = try_interpret_buffer_magic<gps::timestamping::SubscribeRequest>(pb);
    std::cout << " - - - has_value: " << inter.has_value() << std::endl;

    m_sock_timestamping.send(m_device_address, pb);
}

void GPS::request_status()
{
    gps::positioning::Request request;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_sock_position.send(m_device_address, body);
}

void GPS::socket_listener_positioning()
{
    ISocketUserSide::IncomingMessage incoming = *m_sock_position.get();

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

void GPS::socket_listener_timestamp()
{    
    ISocketUserSide::IncomingMessage incoming = *m_sock_timestamping.get();
    // Parsing if SubscribeResponse
    {
        std::optional<gps::timestamping::SubscribeResponse> resp_subscribe = try_interpret_buffer_magic<gps::timestamping::SubscribeResponse>(incoming.data);
        if (resp_subscribe)
        {
            if (m_on_subscribed)
                m_on_subscribed(resp_subscribe->success);
            return;
        }
    }

    {
        // Parsing if TimestampingData
        std::optional<gps::timestamping::TimestampingData> timestamping = try_interpret_buffer_magic<gps::timestamping::TimestampingData>(incoming.data);
        if (timestamping)
        {
            if (!m_on_timestamping)
                return;
            // @TODO Fill the position structure
            Position pos;
            pos.has_pps = true;
            m_on_timestamping(pos);
            return;
        }
    }
}
