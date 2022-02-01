#include "py-physical-layer.hpp"

#include "wideusb/communication/i-physical-layer.hpp"
#include "wideusb-pc/physical-layer-serial-port.hpp"
#include "wideusb-pc/physical-layer-tcp-client.hpp"
#include "wideusb/communication/networking.hpp"

#include "wideusb/communication/binary/channel.hpp"
#include "wideusb/communication/binary/network.hpp"
#include "wideusb/communication/binary/transport.hpp"

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

    void set_on_data_callback(std::function<void(void)> callback) override
    {
        PYBIND11_OVERRIDE_PURE(
            void,            // Return type
            IPhysicalLayer,  // Parent class
            set_on_data_callback,            // Name of function in C++ (must match Python name)
            callback             // Argument(s)
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
        .def(py::init([](std::shared_ptr<IOServiceRunner> io_service_runner, const std::string& port, int baudrate)
             {
                 return std::make_shared<PhysicalLayerSerialPort>(io_service_runner, port, baudrate);
             }), py::arg("io_service_runner"), py::arg("port"), py::arg("baudrate"));
}

void add_tcp_physical_layer(pybind11::module& m)
{
    py::class_<PhysicalLayerTcpClient, std::shared_ptr<PhysicalLayerTcpClient>, IPhysicalLayer>(m, "PhysicalLayerTcpClient")
        .def(py::init([](std::shared_ptr<IOServiceRunner> io_service_runner, const std::string& addr, int port)
             {
                 /// @todo Assert port for number
                 return std::make_shared<PhysicalLayerTcpClient>(io_service_runner, addr, port);
             }), py::arg("io_service_runner"), py::arg("addr"), py::arg("port"));
}
