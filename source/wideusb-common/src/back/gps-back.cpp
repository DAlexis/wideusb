#include "wideusb/back/gps-back.hpp"
#include "wideusb/communication/modules/gps.hpp"

GPSModuleBack::GPSModuleBack(NetService::ptr net_service, Address module_address) :
    m_sock_positioning(net_service, module_address, ports::gps::positioning, [this](ISocketUserSide&) { socket_listener_positioning(); }),
    m_sock_timestamping(net_service, module_address, ports::gps::timestamping, [this](ISocketUserSide&) { socket_listener_timestamping(); })
{
}

void GPSModuleBack::socket_listener_positioning()
{
    Point p = point();
    //tm time_data = p.get_tm();
    gps::positioning::Response resp;
    timestamping_data_from_point(resp.pos_time, p);

    PBuffer resp_buffer = Buffer::create(sizeof(resp), &resp);

    while (m_sock_positioning.has_incoming())
    {
        Socket::IncomingMessage incoming = *m_sock_positioning.get_incoming();

        gps::positioning::Request request;
        if (incoming.data->size() != sizeof(request))
            continue;

        BufferAccessor(incoming.data) >> request;

        m_sock_positioning.send(incoming.sender, resp_buffer);
    }
}

void GPSModuleBack::socket_listener_timestamping()
{
    while (m_sock_timestamping.has_incoming())
    {
        Socket::IncomingMessage incoming = *m_sock_timestamping.get_incoming();

        auto request = try_interpret_buffer_magic<gps::timestamping::SubscribeRequest>(incoming.data);
        if (request.has_value())
        {

            char success = 0;

            if (request->action == gps::timestamping::SubscribeRequest::add)
            {

                if (m_subscribers.size() < 20)
                {
                    success = 1;
                    m_subscribers.insert(Address(request->subscriber));
                }
            } else if (request->action == gps::timestamping::SubscribeRequest::remove)
            {
                auto it = m_subscribers.find(Address(request->subscriber));
                if (it != m_subscribers.end())
                {
                    m_subscribers.erase(it);
                    success = 1;
                }
            }

            gps::timestamping::SubscribeResponse response;
            response.subscriber = request->subscriber;
            response.success = success;
            m_sock_timestamping.send(incoming.sender, Buffer::serialize(response));
        }
    }
}


void GPSModuleBack::timestamping_data_from_point(gps::PosTime& pos_time, const Point& p)
{
    pos_time.latitude = p.latitude;
    pos_time.longitude = p.longitude;
    pos_time.altitude = p.altitude;
    if (p.has_pps)
    {
        pos_time.seconds = p.time.tv_sec;
        pos_time.nanoseconds = p.time.tv_nsec;
    }
}

void GPSModuleBack::send_point_to_subscribers(const Point& p)
{
    gps::timestamping::TimestampingData data;
    timestamping_data_from_point(data.pos_time, p);
    for (Address addr : m_subscribers)
    {
        m_sock_timestamping.send(addr, Buffer::serialize(data));
    }
}
