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
    return pos_to_map(waiter.wait());
}

bool PyGPS::subscribe_to_timestamping()
{
    Waiter<bool> waiter;
    m_gps->subscribe_to_timestamping(waiter.get_waiter_callback(), [this](WideUSBHost::modules::GPS::Position pos) { on_timestamping(pos); });
    return waiter.wait();
}

std::vector<std::map<std::string, std::string>> PyGPS::timestamps()
{
    std::vector<std::map<std::string, std::string>> result;
    for (const auto& pos : m_positions)
    {
        result.push_back(pos_to_map(pos));
    }
    m_positions.clear();
    return result;
}

void PyGPS::on_timestamping(WideUSBHost::modules::GPS::Position pos)
{
    m_positions.push_back(pos);
}

std::map<std::string, std::string> PyGPS::pos_to_map(const WideUSBHost::modules::GPS::Position& pos)
{
    std::map<std::string, std::string> result;
    result["latitude"] = std::to_string(pos.latitude);
    result["longitude"] = std::to_string(pos.longitude);
    result["altitude"] = std::to_string(pos.altitude);
    result["seconds"] = std::to_string(double(pos.nanoseconds) * 1e-9 + double(pos.seconds));
    return result;
}
