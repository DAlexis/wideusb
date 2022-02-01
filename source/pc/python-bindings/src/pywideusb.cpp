#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "py-physical-layer.hpp"
#include "py-asio-utils.hpp"
#include "py-net-service.hpp"
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

    add_io_service_runner(m);

    add_i_physical_layer(m);
    add_usb_physical_layer(m);
    add_tcp_physical_layer(m);

    add_package_inspector(m);
    add_net_service(m);
    add_device_discovery(m);

    add_monitor(m);
    add_gps(m);
    add_dac(m);

}
