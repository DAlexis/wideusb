#include "py-net-service.hpp"

#include "wideusb-common/communication/networking.hpp"

#include "wideusb-common/communication/binary/channel.hpp"
#include "wideusb-common/communication/binary/network.hpp"
#include "wideusb-common/communication/binary/transport.hpp"

#include "wideusb-common/front/discovery.hpp"

#include "wideusb/physical-layer-asio.hpp"

#include "pybind11/stl.h"

namespace py = pybind11;

void add_net_service(pybind11::module& m)
{
    py::class_<NetService, std::shared_ptr<NetService>>(m, "NetService")
         .def(py::init([](std::shared_ptr<IPhysicalLayer> physical)
              {
                  auto phys_layer_asio = std::dynamic_pointer_cast<PhysicalLayerAsio>(physical);
                  if (!phys_layer_asio)
                      throw std::invalid_argument("NetService from python accept only ASIO-based physical layer");

                  return std::make_shared<NetService>(
                      phys_layer_asio,
                      std::make_shared<ChannelLayerBinary>(),
                      std::make_shared<NetworkLayerBinary>(),
                      std::make_shared<TransportLayerBinary>(),
                      nullptr,
                      [phys_layer_asio]() { phys_layer_asio->post_serve_sockets(); });
            }), py::arg("physical_layer"));
}

void add_device_discovery(pybind11::module& m)
{
    py::class_<DeviceDiscovery>(m, "DeviceDiscovery")
         .def(py::init<NetService&, Address>(), py::arg("net_srv"), py::arg("local_address"))
         .def("run", [](DeviceDiscovery& discovery) { discovery.run(); })
         .def("devices", &DeviceDiscovery::devices)
    ;
}
