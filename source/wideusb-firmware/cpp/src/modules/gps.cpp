#include "modules/gps.hpp"
#include "host-communication-interface.hpp"

#include "modules/gps/precision-timer.hpp"
#include "modules/gps/nmea-receiver.hpp"
#include "communication/modules/gps.hpp"

#include "usart.h"

GPSModule::GPSModule(NetSevice& net_service, Address module_address) :
    m_sock_positioning(net_service, module_address, ports::gps::positioning, [this](ISocketUserSide&) { socket_listener_positioning(); }),
    m_sock_timestamping(net_service, module_address, ports::gps::timestamping, [this](ISocketUserSide&) { socket_listener_timestamping(); })
{
    enable();
}

GPSModule::~GPSModule() = default;

void GPSModule::enable()
{
    m_nmea_receiver.reset(new NMEAReceiver(&huart3));
    m_precision_timer.reset(
        new PrecisionTimer(
            &htim2,
            [this](bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps)
            {
                on_precision_timer_signal(has_timing, last_second_duration, ticks_since_pps);
            },
            [this](uint32_t)
            {
                m_nmea_receiver->interrupt_pps();
            }
        )
    );
    m_check_pps_thread.run();
}

Point GPSModule::point()
{
    Point p = m_nmea_receiver->gps().point();
    auto t = m_precision_timer->fract_time();
    if (t.has_value())
    {
        p.fracional_sec = *t;
        p.has_pps = true;
    }
    return p;
}

void GPSModule::tick()
{
    if (!m_points_queue.empty())
    {
        Point p;
        m_points_queue.pop_front(p);
        gps::timestamping::TimestampingData data;
        data.has_pps = p.has_pps;
        for (Address addr : m_subscribers)
        {
            m_sock_timestamping.send(addr, Buffer::serialize(data));
        }
    }
}

void GPSModule::on_precision_timer_signal(bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps)
{
    auto p = m_nmea_receiver->gps().point();
    p.fracional_sec = float(ticks_since_pps) / last_second_duration;
    p.has_pps = has_timing;
    m_points_queue.push_back_from_ISR(p);
}

void GPSModule::check_pps_thread()
{
    for (;;)
    {
        os::delay(100);
        if (m_precision_timer)
            m_precision_timer->check_for_pps_loss();
    }
}

void GPSModule::socket_listener_positioning()
{
    Point p = point();
    //tm time_data = p.get_tm();
    gps::positioning::Response resp;
    resp.latitude = p.latitude;
    resp.longitude = p.longitude;
    resp.has_pps = p.has_pps;

    PBuffer resp_buffer = Buffer::create(sizeof(resp), &resp);

    while (m_sock_positioning.has_data())
    {
        Socket::IncomingMessage incoming = *m_sock_positioning.get();

        gps::positioning::Request request;
        if (incoming.data->size() != sizeof(request))
            continue;

        BufferAccessor(incoming.data) >> request;

        m_sock_positioning.send(incoming.sender, resp_buffer);
    }
}

void GPSModule::socket_listener_timestamping()
{
    while (m_sock_timestamping.has_data())
    {
        Socket::IncomingMessage incoming = *m_sock_timestamping.get();

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

/*
std::unique_ptr<rapidjson::Document> GPSModule::point_to_msg(const Point& p)
{
    using namespace rapidjson;

    struct tm t = p.get_tm();

    std::unique_ptr<Document> d(new Document);
    d->SetObject();
    auto & alloc = d->GetAllocator();

    Value point(kObjectType);

    Value has_position(p.has_pps);
    point.AddMember("has_position", has_position, alloc);

    if (p.has_pps)
    {
        Value latitude(p.latitude);
        point.AddMember("latitude", latitude, alloc);

        Value longitude(p.longitude);
        point.AddMember("longitude", longitude, alloc);

        Value altitude(p.altitude);
        point.AddMember("altitude", altitude, alloc);
    }

    Value sec_epoch(p.time.tv_sec);
    point.AddMember("sec_epoch", sec_epoch, alloc);

    Value year(t.tm_year + 1900);
    point.AddMember("year", year, alloc);

    Value month(t.tm_mon);
    point.AddMember("month", month, alloc);

    Value day(t.tm_mday);
    point.AddMember("day", day, alloc);

    Value hour(t.tm_hour);
    point.AddMember("hour", hour, alloc);

    Value min(t.tm_min);
    point.AddMember("min", min, alloc);

    float sec_val = t.tm_sec;
    if (p.has_pps)
    {
        sec_val += p.fracional_sec;
    }

    Value sec(sec_val);
    point.AddMember("sec", sec, alloc);

    d->AddMember("point", point, alloc);

    return d;
}*/
