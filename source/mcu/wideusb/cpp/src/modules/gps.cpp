#include "modules/gps.hpp"

#include "modules/gps/precision-timer.hpp"
#include "modules/gps/nmea-receiver.hpp"

#include "usart.h"

GPSModule::GPSModule() :
    Module("gps")
{
}

GPSModule::~GPSModule() = default;

void GPSModule::receive_message(const rapidjson::Document& doc)
{
}

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
            [this](uint32_t last_second_duration)
            {
                m_nmea_receiver->interrupt_pps();
            }
        )
    );
    m_check_pps_thread.run();
}

void GPSModule::on_precision_timer_signal(bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps)
{
    auto p = m_nmea_receiver->gps().point();
    p.fracional_sec = float(ticks_since_pps) / last_second_duration;
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
