#include "wideusb/front/gps-front.hpp"

#include "wideusb/communication/modules/gps.hpp"
#include <iostream>

GPSFront::GPSFront(NetService::ptr host_connection_service, OnModuleCreatedCallbackEntry on_created, Address my_address, Address device_address) :
    ModuleFrontBase(
        host_connection_service, gps::id,
        device_address,
        my_address,
        on_created
    ),
    m_sock_position(
        m_host_connection_service,
        my_address,
        ports::gps::positioning,
        [this](ISocketUserSide&) { socket_listener_positioning(); }
    ),
    m_sock_timestamping(
        m_host_connection_service,
        my_address,
        gps::timestamping::port,
        [this](ISocketUserSide&) { socket_listener_timestamp(); }
    )
{
}

void GPSFront::get_position_async(PositionReceivedCallbackEntryPoint callback)
{
    m_on_status_updated = callback;
    request_status();
}

void GPSFront::subscribe_to_timestamping(SubscribedCallbackEntryPoint on_subscribe, TimestampingCallbackEntryPoint on_timestamp)
{
    m_on_subscribed = on_subscribe;
    m_on_timestamping = on_timestamp;

    gps::timestamping::SubscribeRequest subscribe_req;
    subscribe_req.action = gps::timestamping::SubscribeRequest::add;
    subscribe_req.subscriber = m_host_address;

//    std::cout << "Sizeof(subscribe_req) = " << sizeof(subscribe_req) << std::endl;

    PBuffer pb = Buffer::serialize(subscribe_req);
//    auto inter = try_interpret_buffer_magic<gps::timestamping::SubscribeRequest>(pb);
//    std::cout << " - - - has_value: " << inter.has_value() << std::endl;

    m_sock_timestamping.send(m_device_address, pb);
}

void GPSFront::request_status()
{
    gps::positioning::Request request;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_sock_position.send(m_device_address, body);
}

void GPSFront::socket_listener_positioning()
{
    ISocketUserSide::IncomingMessage incoming = *m_sock_position.get_incoming();

    auto response = try_interpret_buffer_no_magic<gps::positioning::Response>(incoming.data);

    if (!response.has_value())
        return;

    Position result;

    position_from_postime(result, response->pos_time);

    m_on_status_updated(result);
}

void GPSFront::socket_listener_timestamp()
{
    ISocketUserSide::IncomingMessage incoming = *m_sock_timestamping.get_incoming();
    // Parsing if SubscribeResponse

    std::optional<gps::timestamping::SubscribeResponse> resp_subscribe = try_interpret_buffer_magic<gps::timestamping::SubscribeResponse>(incoming.data);
    if (resp_subscribe)
    {
        m_on_subscribed(resp_subscribe->success);
        return;
    }

    // Parsing if TimestampingData
    std::optional<gps::timestamping::TimestampingData> timestamping = try_interpret_buffer_magic<gps::timestamping::TimestampingData>(incoming.data);
    if (timestamping)
    {
        if (!m_on_timestamping.avaliable())
            return;

        Position pos;
        position_from_postime(pos, timestamping->pos_time);
        m_on_timestamping(pos);
        return;
    }

}

void GPSFront::position_from_postime(GPSFront::Position& position, const gps::PosTime& postime)
{
    position.latitude = postime.latitude;
    position.longitude = postime.longitude;
    position.altitude = postime.altitude;

    position.seconds = postime.seconds;
    position.nanoseconds = postime.nanoseconds;
}
