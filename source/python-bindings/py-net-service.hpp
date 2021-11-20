#ifndef PYNETSERVICE_HPP
#define PYNETSERVICE_HPP

#include "pybind11/pybind11.h"

void add_net_service(pybind11::module& m);
void add_device_discovery(pybind11::module& m);

#endif // PYNETSERVICE_HPP
