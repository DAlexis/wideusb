#ifndef PYGPS_HPP
#define PYGPS_HPP

#include "host-modules/gps.hpp"
#include "py-wideusb-device.hpp"

class PyGPS
{
public:
    PyGPS(PyWideUSBDevice& device, Address custom_host_address = 0, Address custom_device_address = 0);

    std::map<std::string, std::string> position();

private:

    std::unique_ptr<WideUSBHost::modules::GPS> m_gps;
};

#endif // PYGPS_HPP
