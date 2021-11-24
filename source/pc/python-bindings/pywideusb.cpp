#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "py-physical-layer.hpp"
#include "py-asio-utils.hpp"
#include "py-net-service.hpp"
#include "py-wideusb-device.hpp"
#include "py-monitor.hpp"
#include "py-gps.hpp"
#include "py-dac.hpp"

#include <iostream>

#include <stdlib.h>
#include <time.h>

namespace py = pybind11;

PYBIND11_MODULE(pywideusb, m) {
    srand(time(0));

    m.doc() = "Python bindings for wideusb device";

    add_io_service(m);
    add_io_service_runner(m);

    add_i_physical_layer(m);
    add_usb_physical_layer(m);
    add_tcp_physical_layer(m);

    add_net_service(m);
    add_device_discovery(m);

    py::class_<PyWideUSBDevice>(m, "Device")
        .def(py::init<uint64_t, std::string, int>(),
                     py::arg("host_address") = 0x87654321,
                     py::arg("port") = "/dev/ttyACM0",
                     py::arg("baudrate") = 921600
                     )
        .def("device_address", &PyWideUSBDevice::device_address);


    py::class_<PyMonitor>(m, "Monitor")
        .def(py::init<NetService&, Address, Address>(),
             py::arg("net_service"),
             py::arg("local_address") = 0,
             py::arg("remote_address") = 0)
        .def("status", &PyMonitor::status);

    py::class_<PyGPS>(m, "GPS")
        .def(py::init<PyWideUSBDevice&, Address, Address>(),
             py::arg("device"),
             py::arg("custom_host_address") = 0,
             py::arg("custom_device_address") = 0)
        .def("position", &PyGPS::position)
        .def("subscribe_to_timestamping", &PyGPS::subscribe_to_timestamping)
        .def("timestamps", &PyGPS::timestamps);

    py::class_<PyDAC>(m, "DAC")
        .def(py::init<PyWideUSBDevice&, Address, Address>(),
             py::arg("device"),
             py::arg("custom_host_address") = 0,
             py::arg("custom_device_address") = 0)
        .def("init_sample", &PyDAC::init_sample, py::arg("buffer_size"), py::arg("prescaler"), py::arg("period"), py::arg("repeat"))
        .def("run", &PyDAC::run)
        .def("send_data", &PyDAC::send_data, py::arg("data"));
}
