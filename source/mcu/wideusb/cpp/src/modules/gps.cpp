#include "modules/gps.hpp"
#include "host-communication-interface.hpp"

#include "modules/gps/precision-timer.hpp"
#include "modules/gps/nmea-receiver.hpp"

#include "usart.h"

GPSModule::GPSModule() :
    Module("gps")
{
    m_action_filter.add(
        "get_point",
        [this](const rapidjson::Value&) -> ErrorCode
        {
            send_point();
            return std::nullopt;
        }
    );
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
            [this](uint32_t /*last_second_duration*/)
            {
                m_nmea_receiver->interrupt_pps();
            }
        )
    );
    m_check_pps_thread.run();
    m_send_timings_thread.run();
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

void GPSModule::send_signal_timings_thread()
{
    Point p;
    for (;;)
    {
        m_points_queue.pop_front(p);
        auto doc = point_to_msg(p);
        add_module_field(*doc);
        m_communicator->send_data(std::move(doc));
    }
}

void GPSModule::send_point()
{
    Point p = point();
    auto doc = point_to_msg(p);
    add_module_field(*doc);
    m_communicator->send_data(std::move(doc));
}

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
}
