#include "modules/gps.hpp"

#include "modules/gps/precision-timer.hpp"
#include "modules/gps/nmea-receiver.hpp"

GPSModule::GPSModule()
{
    m_task_check_pps.set_task(
        [this]()
        {
            if (m_precision_timer)
                m_precision_timer->check_for_pps_loss();
        }
    );
    m_task_check_pps.set_stack_size(128);
}

GPSModule::~GPSModule() = default;

const char* GPSModule::name()
{
    return "gps";
}

void GPSModule::receive_message(const rapidjson::Document& doc)
{
}

void GPSModule::enable()
{
    m_precision_timer.reset(
        new PrecisionTimer(
            &htim2,
            [this](bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps)
            {
                on_precision_timer_signal(has_timing, last_second_duration, ticks_since_pps);
            }
        )
    );
    m_task_check_pps.run(100);
}

void GPSModule::on_precision_timer_signal(bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps)
{
}
