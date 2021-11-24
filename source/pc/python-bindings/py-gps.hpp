#ifndef PYGPS_HPP
#define PYGPS_HPP

#include "wideusb/front/gps-front.hpp"
#include "py-wideusb-device.hpp"

class PyGPS
{
public:
    PyGPS(PyWideUSBDevice& device, Address custom_host_address = 0, Address custom_device_address = 0);

    std::map<std::string, std::string> position();
    bool subscribe_to_timestamping();
    std::vector<std::map<std::string, std::string>> timestamps();

private:

    static std::map<std::string, std::string> pos_to_map(const GPSFront::Position& pos);

    void on_timestamping(GPSFront::Position pos);

    std::unique_ptr<GPSFront> m_gps;

    std::vector<GPSFront::Position> m_positions;
};

#endif // PYGPS_HPP
