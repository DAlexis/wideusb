#ifndef GPS_HPP
#define GPS_HPP


#include "communication/networking.hpp"
#include "host-module.hpp"

#include <string>
#include <optional>

namespace WideUSBHost
{

class Device;

namespace modules
{

class GPS : public ModuleBase
{
public:
    struct Position
    {
        float latitude = 0;
        float longitude = 0;
        bool has_pps = false;

        // @TODO Add timestamp-related fields
        float seconds_partial;
    };


    using PositionReceivedCallback = std::function<void(Position)>;
    using SubscribedCallback = std::function<void(bool)>;
    using TimestampingCallback = std::function<void(Position)>;

    GPS(Device& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address = 0, Address custom_device_address = 0);

    void get_position_async(PositionReceivedCallback callback);
    void subscribe_to_timestamping(SubscribedCallback on_subscribe, TimestampingCallback on_timestamp);

private:
    void request_status();
    void socket_listener_positioning();
    void socket_listener_timestamp();

    Socket m_sock_position;
    Socket m_sock_timestamping;

    PositionReceivedCallback m_on_status_updated;
    SubscribedCallback m_on_subscribed;

    TimestampingCallback m_on_timestamping;
};

}

}

#endif // GPS_HPP
