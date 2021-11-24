#ifndef GPSFRONT_HPP
#define GPSFRONT_HPP

#include "wideusb/front/base-front.hpp"
#include "wideusb/communication/networking.hpp"

#include <string>
#include <optional>

namespace gps { struct PosTime; }

class GPSFront : public ModuleFrontBase
{
public:
    struct Position
    {
        float latitude = 0;
        float longitude = 0;
        float altitude = 0;

        uint64_t seconds = 0;
        uint32_t nanoseconds = 0;
    };


    using PositionReceivedCallback = std::function<void(Position)>;
    using SubscribedCallback = std::function<void(bool)>;
    using TimestampingCallback = std::function<void(Position)>;

    GPSFront(NetService& host_connection_service, OnModuleCreatedCallback on_created, Address my_address, Address device_address);

    void get_position_async(PositionReceivedCallback callback);
    void subscribe_to_timestamping(SubscribedCallback on_subscribe, TimestampingCallback on_timestamp);

private:
    void request_status();
    void socket_listener_positioning();
    void socket_listener_timestamp();

    static void position_from_postime(Position& position, const gps::PosTime& postime);

    Socket m_sock_position;
    Socket m_sock_timestamping;

    PositionReceivedCallback m_on_status_updated;
    SubscribedCallback m_on_subscribed;

    TimestampingCallback m_on_timestamping;
};

#endif // GPSFRONT_HPP
