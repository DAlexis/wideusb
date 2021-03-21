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
    };

    using PositionReceivedCallback = std::function<void(Position)>;
    GPS(Device& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address = 0, Address custom_device_address = 0);

    void get_position_async(PositionReceivedCallback callback);

private:
    void request_status();
    void socket_listener();

    Socket m_position_socket;
    PositionReceivedCallback m_on_status_updated;
};

}

}

#endif // GPS_HPP
