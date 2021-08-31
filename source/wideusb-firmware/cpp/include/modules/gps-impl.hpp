#ifndef GPS_HPP_UNCLUDED
#define GPS_HPP_UNCLUDED

#include "module-base.hpp"
#include "wideusb-common/utils/gps/nmea-parser.hpp"
#include "os/cpp-freertos.hpp"
#include "communication/networking.hpp"
#include "wideusb-common/back/gps-back.hpp"

#include <memory>
#include <set>

class PrecisionTimer;
class NMEAReceiver;

namespace gps { struct PosTime; }

class GPSImpl : public GPSModuleBack, public IModule
{
public:
    GPSImpl(NetSevice& net_service, Address module_address);
    ~GPSImpl();

    void enable();
    Point point() override;

    void tick() override;

private:
    void on_precision_timer_signal(bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps);
    void check_pps_thread();

    std::unique_ptr<PrecisionTimer> m_precision_timer;
    std::unique_ptr<NMEAReceiver> m_nmea_receiver;
    os::Thread m_check_pps_thread {[this](){ check_pps_thread(); }, "PPS_check", 128};
    os::Queue<Point> m_points_queue;

    std::set<Address> m_subscribers;
};

#endif // GPS_HPP_UNCLUDED
