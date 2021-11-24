#ifndef GPSBACK_HPP
#define GPSBACK_HPP


#include "wideusb/communication/networking.hpp"
#include "wideusb/utils/point.hpp"
#include "wideusb/back/base-back.hpp"

namespace gps { struct PosTime; }

class GPSModuleBack : public IModule
{
public:
    GPSModuleBack(NetService& net_service, Address module_address);

    virtual Point point() = 0;

protected:
    static void timestamping_data_from_point(gps::PosTime& pos_time, const Point& p);
    void send_point_to_subscribers(const Point& p);

    Socket m_sock_positioning;
    Socket m_sock_timestamping;

    std::set<Address> m_subscribers;

private:

    void socket_listener_positioning();
    void socket_listener_timestamping();
};


#endif // GPSBACK_HPP
