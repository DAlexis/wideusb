#ifndef GPS_HPP_UNCLUDED
#define GPS_HPP_UNCLUDED

#include "module-base.hpp"
#include "gps/nmea-parser.hpp"
#include "os/cpp-freertos.hpp"
#include "communication/networking.hpp"

#include <memory>
#include <set>

class PrecisionTimer;
class NMEAReceiver;

class GPSModule : public IModule
{
public:
    GPSModule(NetSevice& net_service, Address monitor_address);
    ~GPSModule();

    void enable();
    Point point();

    void tick() override;

private:

    void on_precision_timer_signal(bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps);
    void check_pps_thread();

    void socket_listener_positioning();
    void socket_listener_timestamping();

    //static std::unique_ptr<rapidjson::Document> point_to_msg(const Point& p);

    std::unique_ptr<PrecisionTimer> m_precision_timer;
    std::unique_ptr<NMEAReceiver> m_nmea_receiver;
    os::Thread m_check_pps_thread {[this](){ check_pps_thread(); }, "PPS_check", 128};
    os::Queue<Point> m_points_queue;

    Socket m_sock_positioning;
    Socket m_sock_timestamping;

    std::set<Address> m_subscribers;
};

#endif // GPS_HPP_UNCLUDED
