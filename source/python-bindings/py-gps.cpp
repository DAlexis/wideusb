#include "py-gps.hpp"

PyGPS::PyGPS(PyWideUSBDevice& device, Address custom_host_address, Address custom_device_address)
{
    Waiter<bool> waiter;
    m_gps.reset(new WideUSBHost::modules::GPS(device.device(), waiter.get_waiter_callback(), custom_host_address, custom_device_address));
    bool success = waiter.wait();
    if (!success)
        throw std::runtime_error("GPS module creation failed");
}

std::map<std::string, std::string> PyGPS::position()
{
    std::map<std::string, std::string> result;
    Waiter<WideUSBHost::modules::GPS::Position> waiter;
    m_gps->get_position_async(waiter.get_waiter_callback());
    WideUSBHost::modules::GPS::Position response = waiter.wait();
    result["latitude"] = std::to_string(response.latitude);
    result["longitude"] = std::to_string(response.longitude);
    result["has_pps"] = std::to_string(response.has_pps);
    return result;
}
