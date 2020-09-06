#ifndef GPS_HPP_UNCLUDED
#define GPS_HPP_UNCLUDED

#include "gps/nmea-parser.hpp"
#include "os/cpp-freertos.hpp"

#include "module.hpp"
#include <memory>

class PrecisionTimer;
class NMEAReceiver;

class GPSModule : public Module
{
public:
    GPSModule();
    ~GPSModule();

    const char* name() override;
    void receive_message(const rapidjson::Document& doc) override;

    void enable();

private:

    void on_precision_timer_signal(bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps);

    std::unique_ptr<PrecisionTimer> m_precision_timer;
    std::unique_ptr<NMEAReceiver> m_nmea_receiver;
    os::TaskCycled m_task_check_pps{ nullptr, "PPS_check"};
    os::Queue<Point> m_points_queue;

};

#endif // GPS_HPP_UNCLUDED
