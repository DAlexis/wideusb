#include "modules/gps-impl.hpp"

#include "modules/gps/precision-timer.hpp"
#include "modules/gps/nmea-receiver.hpp"
#include "wideusb/communication/modules/gps.hpp"

#include "usart.h"

GPSImpl::GPSImpl(NetService::ptr net_service, Address module_address) :
    GPSModuleBack(net_service, module_address)
{
    enable();
}

GPSImpl::~GPSImpl() = default;

void GPSImpl::enable()
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

Point GPSImpl::point()
{
    Point p = m_nmea_receiver->gps().point();
    auto t = m_precision_timer->fract_time();
    if (t.has_value())
    {
        p.time.tv_nsec = *t * 1e9;
        p.has_pps = true;
    }
    return p;
}

void GPSImpl::tick()
{
    if (!m_points_queue.empty())
    {
        Point p = *m_points_queue.pop_front();
        send_point_to_subscribers(p);
    }
}

void GPSImpl::on_precision_timer_signal(bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps)
{
    auto p = m_nmea_receiver->gps().point();
    p.time.tv_nsec = float(ticks_since_pps) / last_second_duration * 1e9;
    p.has_pps = has_timing;
    m_points_queue.push_back_from_ISR(p);
}

void GPSImpl::check_pps_thread()
{
    for (;;)
    {
        os::delay(100ms);
        if (m_precision_timer)
            m_precision_timer->check_for_pps_loss();
    }
}
