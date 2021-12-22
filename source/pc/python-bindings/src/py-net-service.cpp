#include "py-net-service.hpp"

#include "wideusb/communication/networking.hpp"

#include "wideusb/communication/binary/channel.hpp"
#include "wideusb/communication/binary/network.hpp"
#include "wideusb/communication/binary/transport.hpp"

#include "wideusb/front/discovery.hpp"

#include "wideusb-pc/physical-layer-asio.hpp"
#include "wideusb-pc/package-inspector.hpp"

#include "pybind11/stl.h"

namespace py = pybind11;

class PyIPackageInspector : public IPackageInspector {
public:
    // Inherit the constructors
    using IPackageInspector::IPackageInspector;

    void inspect_package(const PBuffer data, const std::string& context_msg) override
    {
        PYBIND11_OVERRIDE_PURE(
            void, // Return type
            IPackageInspector,      // Parent class
            inspect_package,            // Name of function in C++ (must match Python name)
            data, context_msg          // Argument(s)
        );
    }
};


void add_package_inspector(pybind11::module& m)
{
    py::class_<IPackageInspector, std::shared_ptr<IPackageInspector>, PyIPackageInspector>(m, "IPackageInspector");
    py::class_<PackageInspector, std::shared_ptr<PackageInspector>, IPackageInspector>(m, "PackageInspector")
            .def(py::init<>());
}

void add_net_service(pybind11::module& m)
{
    py::class_<NetService, std::shared_ptr<NetService>>(m, "NetService")
         .def(py::init([](std::shared_ptr<IPhysicalLayer> physical, std::shared_ptr<IPackageInspector> package_inspector)
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
                      [phys_layer_asio]() { phys_layer_asio->post_serve_sockets(); },
                      package_inspector);
            }), py::arg("physical_layer"), py::arg("package_inspector") = nullptr);
}

void add_device_discovery(pybind11::module& m)
{
    py::class_<DeviceDiscovery>(m, "DeviceDiscovery")
         .def(py::init<NetService&, Address>(), py::arg("net_srv"), py::arg("local_address"))
         .def("run", [](DeviceDiscovery& discovery) { discovery.run(); })
         .def("devices", &DeviceDiscovery::devices)
    ;
}
