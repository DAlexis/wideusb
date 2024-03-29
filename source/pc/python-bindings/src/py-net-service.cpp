#include "py-net-service.hpp"

#include "wideusb/communication/networking.hpp"

#include "wideusb/communication/binary/channel.hpp"
#include "wideusb/communication/binary/network.hpp"
#include "wideusb/communication/binary/transport.hpp"

#include "wideusb/front/discovery.hpp"

#include "wideusb-pc/physical-layer-asio.hpp"
#include "wideusb-pc/package-inspector.hpp"
#include "wideusb-pc/socket-queue-mutex.hpp"
#include "wideusb-pc/asio-net-srv-runner.hpp"

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
/*
void add_net_interface(pybind11::module& m)
{
    py::class_<NetworkInterface, std::shared_ptr<NetworkInterface>>(m, "NetworkInterface")
            .def(py::init([](std::shared_ptr<IPhysicalLayer> physical) {
                     return std::make_shared<NetworkInterface>(physical, std::make_shared<ChannelLayerBinary>(), true);
                 }), py::arg("physical"));
}*/

void add_net_service(pybind11::module& m)
{
    py::class_<NetService, std::shared_ptr<NetService>>(m, "NetService")
         .def(py::init([](std::shared_ptr<IOServiceRunner> io_service_runner, std::shared_ptr<IPhysicalLayer> physical, std::shared_ptr<IPackageInspector> package_inspector)
              {
                  auto phys_layer_asio = std::dynamic_pointer_cast<PhysicalLayerAsio>(physical);
                  if (!phys_layer_asio)
                      throw std::invalid_argument("NetService from python accept only ASIO-based physical layer");

                  auto net_srv = NetService::create(
                      std::move(NetServiceRunnerAsio::create(io_service_runner)),
                      std::make_shared<MutexQueueFactory>(),
                      std::make_shared<NetworkLayerBinary>(),
                      std::make_shared<TransportLayerBinary>(),
                      package_inspector);

                  net_srv->add_interface(std::make_shared<NetworkInterface>(
                      phys_layer_asio,
                      std::make_shared<ChannelLayerBinary>(),
                      false));
                  return net_srv;
            }), py::arg("io_service_runner"), py::arg("physical_layer"), py::arg("package_inspector") = nullptr);
}

void add_device_discovery(pybind11::module& m)
{
    py::class_<DeviceDiscovery>(m, "DeviceDiscovery")
         .def(py::init<NetService::ptr, Address>(), py::arg("net_srv"), py::arg("local_address"))
         .def("run", [](DeviceDiscovery& discovery) { discovery.run(); })
         .def("devices", &DeviceDiscovery::devices)
    ;
}
