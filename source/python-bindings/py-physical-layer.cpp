#include "py-physical-layer.hpp"

#include "wideusb-common/communication/i-physical-layer.hpp"
#include "wideusb/physical-layer-serial-port.hpp"
#include "wideusb/physical-layer-tcp-client.hpp"
#include "wideusb-common/communication/networking.hpp"

#include "wideusb-common/communication/binary/channel.hpp"
#include "wideusb-common/communication/binary/network.hpp"
#include "wideusb-common/communication/binary/transport.hpp"

namespace py = pybind11;

class PyIPhysicalLayer : public IPhysicalLayer {
public:
    // Inherit the constructors
    using IPhysicalLayer::IPhysicalLayer;

    SerialReadAccessor& incoming() override
    {
        PYBIND11_OVERRIDE_PURE(
            SerialReadAccessor&, // Return type
            IPhysicalLayer,      // Parent class
            incoming            // Name of function in C++ (must match Python name)
                         // Argument(s)
        );
    }

    void send(PBuffer data) override
    {
        PYBIND11_OVERRIDE_PURE(
            void,            // Return type
            IPhysicalLayer,  // Parent class
            send,            // Name of function in C++ (must match Python name)
            data             // Argument(s)
        );
    }

    void on_network_service_connected(NetService& srv) override
    {
        PYBIND11_OVERRIDE_PURE(
            void,            // Return type
            IPhysicalLayer,  // Parent class
            on_network_service_connected,            // Name of function in C++ (must match Python name)
            srv             // Argument(s)
        );
    }
};

void add_i_physical_layer(pybind11::module& m)
{
    py::class_<IPhysicalLayer, std::shared_ptr<IPhysicalLayer>, PyIPhysicalLayer>(m, "IPhysicalLayer")
        .def(py::init<>());
}

void add_usb_physical_layer(pybind11::module& m)
{
    py::class_<PhysicalLayerSerialPort, std::shared_ptr<PhysicalLayerSerialPort>, IPhysicalLayer>(m, "PhysicalLayerSerialPort")
        .def(py::init([](boost::asio::io_service& io_service, const std::string& port, int baudrate)
             {
                 return std::make_shared<PhysicalLayerSerialPort>(io_service, port, baudrate);
             }), py::arg("io_service"), py::arg("port"), py::arg("baudrate"));
}

void add_tcp_physical_layer(pybind11::module& m)
{
    py::class_<PhysicalLayerTcpClient, std::shared_ptr<PhysicalLayerTcpClient>, IPhysicalLayer>(m, "PhysicalLayerTcpClient")
        .def(py::init([](boost::asio::io_service& io_service, const std::string& addr, int port)
             {
                 /// @todo Assert port for number
                 return std::make_shared<PhysicalLayerTcpClient>(io_service, addr, port);
             }), py::arg("io_service"), py::arg("addr"), py::arg("port"));
}
