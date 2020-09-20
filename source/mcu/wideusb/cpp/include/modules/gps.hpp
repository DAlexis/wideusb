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

    void enable();

private:

    void on_precision_timer_signal(bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps);
    void check_pps_thread();

    void send_point();

    std::unique_ptr<PrecisionTimer> m_precision_timer;
    std::unique_ptr<NMEAReceiver> m_nmea_receiver;
    os::Thread m_check_pps_thread {[this](){ check_pps_thread(); }, "PPS_check", 128};
    os::Queue<Point> m_points_queue;

};

#endif // GPS_HPP_UNCLUDED
