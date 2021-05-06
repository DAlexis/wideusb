#ifndef PYGPS_HPP
#define PYGPS_HPP

#include "host-modules/gps.hpp"
#include "py-wideusb-device.hpp"

class PyGPS
{
public:
    PyGPS(PyWideUSBDevice& device, Address custom_host_address = 0, Address custom_device_address = 0);

    std::map<std::string, std::string> position();
    bool subscribe_to_timestamping();
    std::vector<std::map<std::string, std::string>> timestamps();

private:

    static std::map<std::string, std::string> pos_to_map(const WideUSBHost::modules::GPS::Position& pos);

    void on_timestamping(WideUSBHost::modules::GPS::Position pos);

    std::unique_ptr<WideUSBHost::modules::GPS> m_gps;

    std::vector<WideUSBHost::modules::GPS::Position> m_positions;
};

#endif // PYGPS_HPP
