#include "wideusb-pc/wideusb.hpp"
#include "wideusb/communication/binary/channel.hpp"
#include "wideusb/communication/binary/network.hpp"
#include "wideusb/communication/binary/transport.hpp"
#include "wideusb/communication/modules/ports.hpp"
#include "wideusb/communication/modules/core.hpp"

#include <iostream>
#include <limits>

using namespace WideUSBHost;

using namespace boost::asio;

Device::Device(Address host_address, const std::string& port, OnDeviceDiscoveredCallback on_discovered, int baudrate) :
    m_physical_layer(std::make_shared<PhysicalLayerSerialPort>(
        m_io_service,
        port,
        baudrate)),
    m_net_srv(
        m_physical_layer,
        std::make_shared<ChannelLayerBinary>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>(),
        nullptr,
        [this]() { post_serve_sockets(); }
    ),
    m_serve_sockets_task(m_io_service, 10, [this] { m_net_srv.serve_sockets(std::chrono::steady_clock::now()); return false; }),
    m_host_address(host_address),

    m_create_module_socket(m_net_srv, m_host_address, ports::core::create_module,
                           [this](ISocketUserSide&){ create_module_socket_listener(); }),

    m_on_discovered(on_discovered),
    m_creation(std::chrono::steady_clock::now())
{
    run_device_discovery();
}

void Device::run_device_discovery()
{
    m_device_discovery_socket.reset(
        new Socket(
            m_net_srv, m_host_address, ports::core::address_discovery,
            [this](ISocketUserSide&){ discovery_socket_listener(); }
        )
    );

    m_device_discovery_socket->options().retransmitting_options.cycles_count = 0;
    m_device_discovery_socket->options().retransmitting_options.timeout = 0ms;
    m_device_discovery_socket->options().ttl = 1; // Do not retransmit over network
    m_device_discovery_socket->address_filter().listen_address(0x00000000, 0x00000000); // Any

    core::discovery::Request request;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_device_discovery_socket->send(0xFFFFFFFF, body);
    std::cout << "Sending address request..." << std::endl;
}

void Device::discovery_socket_listener()
{
    Socket::IncomingMessage incoming = *m_device_discovery_socket->get();
    std::cout << "Device discovered!! " << incoming.sender << std::endl;
    m_device_addr = incoming.sender;
    m_device_discovery_socket.reset();
    if (m_on_discovered)
        m_on_discovered();
}

void Device::create_module_socket_listener()
{
}

void Device::run_io_service()
{
    boost::asio::io_service::work work(m_io_service);
    m_io_service.run();
}

void Device::run_single(SingleTask task, size_t milliseconds)
{
    if (milliseconds == 0)
        boost::asio::post(m_io_service, [task]() { task(); });

}

void Device::post_serve_sockets()
{
    boost::asio::post(m_io_service, [this]() { m_net_srv.serve_sockets(std::chrono::steady_clock::now()); });
}

NetService& Device::net_service()
{
    return m_net_srv;
}

boost::asio::io_service& Device::io_service()
{
    return m_io_service;
}

Address Device::device_address()
{
    return m_device_addr;
}

Address Device::host_address()
{
    return m_host_address;
}

bool Device::device_connected()
{
    return m_device_addr != 0;
}
