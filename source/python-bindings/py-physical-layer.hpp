#ifndef PYPHYSICALLAYER_HPP
#define PYPHYSICALLAYER_HPP

#include "pybind11/pybind11.h"

void add_io_service(pybind11::module& m);

void add_i_physical_layer(pybind11::module& m);
void add_usb_physical_layer(pybind11::module& m);
void add_tcp_physical_layer(pybind11::module& m);

void add_net_service(pybind11::module& m);

#endif // PYPHYSICALLAYER_HPP
