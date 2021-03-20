#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "py-wideusb-device.hpp"
#include "py-monitor.hpp"

#include <iostream>

namespace py = pybind11;

PYBIND11_MODULE(pywideusb, m) {
    m.doc() = "Python bindings for wideusb device";

    py::class_<PyWideUSBDevice>(m, "Device")
        .def(py::init<uint64_t, std::string, int>(),
                     py::arg("host_address") = 0x87654321,
                     py::arg("port") = "/dev/ttyACM0",
                     py::arg("baudrate") = 921600
                     )
        .def("device_address", &PyWideUSBDevice::device_address);

    py::class_<PyMonitor>(m, "Monitor")
        .def(py::init<PyWideUSBDevice&, Address, Address>(),
             py::arg("device"),
             py::arg("custom_host_address") = 0,
             py::arg("custom_device_address") = 0)
        .def("status", &PyMonitor::status);
}
