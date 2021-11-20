#ifndef PY_ASIO_UTILS_HPP
#define PY_ASIO_UTILS_HPP

#include "pybind11/pybind11.h"

void add_io_service(pybind11::module& m);
void add_io_service_runner(pybind11::module& m);


#endif // PY_ASIO_UTILS_HPP
