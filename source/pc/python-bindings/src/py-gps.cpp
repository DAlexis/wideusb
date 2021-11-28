#include "py-gps.hpp"

#include "wideusb/front/gps-front.hpp"
#include "wideusb-pc/asio-utils.hpp"

namespace py = pybind11;

class PyGPS
{
public:
    PyGPS(NetService& net_service, Address local_address, Address remote_address);

    std::map<std::string, std::string> position();
    bool subscribe_to_timestamping();
    std::vector<std::map<std::string, std::string>> timestamps();

private:

    static std::map<std::string, std::string> pos_to_map(const GPSFront::Position& pos);

    void on_timestamping(GPSFront::Position pos);

    std::unique_ptr<GPSFront> m_gps;

    std::vector<GPSFront::Position> m_positions;
};

PyGPS::PyGPS(NetService& net_service, Address local_address, Address remote_address)
{
    Waiter<bool> waiter;
    m_gps.reset(new GPSFront(net_service, waiter.get_waiter_callback(), local_address, remote_address));
    bool success = waiter.wait();
    if (!success)
        throw std::runtime_error("GPS module creation failed");
}

std::map<std::string, std::string> PyGPS::position()
{
    std::map<std::string, std::string> result;
    Waiter<GPSFront::Position> waiter;
    m_gps->get_position_async(waiter.get_waiter_callback());
    return pos_to_map(waiter.wait());
}

bool PyGPS::subscribe_to_timestamping()
{
    Waiter<bool> waiter;
    m_gps->subscribe_to_timestamping(waiter.get_waiter_callback(), [this](GPSFront::Position pos) { on_timestamping(pos); });
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

void PyGPS::on_timestamping(GPSFront::Position pos)
{
    m_positions.push_back(pos);
}

std::map<std::string, std::string> PyGPS::pos_to_map(const GPSFront::Position& pos)
{
    std::map<std::string, std::string> result;
    result["latitude"] = std::to_string(pos.latitude);
    result["longitude"] = std::to_string(pos.longitude);
    result["altitude"] = std::to_string(pos.altitude);
    result["seconds"] = std::to_string(double(pos.nanoseconds) * 1e-9 + double(pos.seconds));
    return result;
}

void add_gps(pybind11::module& m)
{
    py::class_<PyGPS>(m, "GPS")
        .def(py::init<NetService&, Address, Address>(),
             py::arg("device"),
             py::arg("local_address"),
             py::arg("remote_address"))
        .def("position", &PyGPS::position)
        .def("subscribe_to_timestamping", &PyGPS::subscribe_to_timestamping)
        .def("timestamps", &PyGPS::timestamps);
}