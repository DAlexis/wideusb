#include "py-asio-utils.hpp"

#include "wideusb-pc/asio-utils.hpp"

namespace py = pybind11;

void add_io_service(pybind11::module& m)
{
    py::class_<boost::asio::io_service>(m, "IOService")
            .def(py::init<>());
}

void add_io_service_runner(pybind11::module& m)
{
    py::class_<IOServiceRunner, std::shared_ptr<IOServiceRunner>>(m, "IOServiceRunner")
            .def(py::init([](){ return IOServiceRunner::create(); }))
            .def("io_service", &IOServiceRunner::io_service, py::return_value_policy::reference_internal);
}
