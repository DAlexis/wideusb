#include "wideusb.hpp"
#include "communication/binary/channel.hpp"
#include "communication/binary/network.hpp"
#include "communication/binary/transport.hpp"
#include "communication/modules/ports.hpp"
#include "communication/modules/core.hpp"

#include <iostream>
#include <limits>

using namespace boost::asio;


WideUSBDevice::WideUSBDevice(Address host_address, const std::string& port, OnDeviceDiscoveredCallback on_discovered, int baudrate) :

    m_physical_layer(std::make_shared<SerialPortPhysicalLayer>(
        m_io_service,
        port,
        [this]() { post_serve_sockets(); },
        baudrate)),
    m_net_srv(
        m_physical_layer,
        std::make_shared<ChannelLayerBinary>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>(),
        nullptr,
        [this]() { post_serve_sockets(); }
    ),
    m_serve_sockets_task(m_io_service, 10, [this] { m_net_srv.serve_sockets(time_ms()); return false; }),
    m_host_address(host_address),

    m_create_module_socket(m_net_srv, m_host_address, ports::core::create_module,
                           [this](ISocketUserSide&){ create_module_socket_listener(); }),

    m_on_discovered(on_discovered),
    m_creation(std::chrono::steady_clock::now())
{
    //m_device_discovery_task.reset(new DeviceDiscoveryTask(m_host_address, m_io_service, m_net_srv, [this](Address addr) { on_device_discovered(addr); } ));
    run_device_discovery();
}

void WideUSBDevice::run_device_discovery()
{
    m_device_discovery_socket.reset(
        new Socket(
            m_net_srv, m_host_address, ports::core::address_discovery,
            [this](ISocketUserSide&){ discovery_socket_listener(); }
        )
    );

    m_device_discovery_socket->options().retransmitting_options.cycles_count = 0;
    m_device_discovery_socket->options().retransmitting_options.timeout = 0;
    m_device_discovery_socket->options().ttl = 1; // Do not retransmit over network
    m_device_discovery_socket->address_filter().listen_address(0x00000000, 0x00000000); // Any

    core::discovery::Request request;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_device_discovery_socket->send(0xFFFFFFFF, body);
    std::cout << "Sending address request..." << std::endl;
}

void WideUSBDevice::discovery_socket_listener()
{
    Socket::IncomingMessage incoming = *m_device_discovery_socket->get();
    std::cout << "Device discovered!! " << incoming.sender << std::endl;
    m_device_addr = incoming.sender;
    m_device_discovery_socket.reset();
    if (m_on_discovered)
        m_on_discovered();
}

void WideUSBDevice::create_module_socket_listener()
{
}

void WideUSBDevice::run_io_service()
{
    for(;;)
    {
        m_io_service.run();
    }
}
/*
void WideUSBDevice::connect_module(IModuleOnHost& module)
{    
    m_host_modules.insert(&module);
    if (m_device_addr != 0)
        module.on_device_ready();
}

void WideUSBDevice::remove_module(IModuleOnHost& module)
{
    m_host_modules.erase(&module);
}*/

void WideUSBDevice::run_single(SingleTask task, size_t milliseconds)
{
    if (milliseconds == 0)
        boost::asio::post(m_io_service, [task]() { task(); });

}


void WideUSBDevice::post_serve_sockets()
{
    boost::asio::post(m_io_service, [this]() { m_net_srv.serve_sockets(time_ms()); });
}

uint32_t WideUSBDevice::time_ms()
{
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_creation).count();
    return duration % std::numeric_limits<uint32_t>::max();
}

NetSevice& WideUSBDevice::net_service()
{
    return m_net_srv;
}

boost::asio::io_service& WideUSBDevice::io_service()
{
    return m_io_service;
}

Address WideUSBDevice::device_address()
{
    return m_device_addr;
}

Address WideUSBDevice::host_address()
{
    return m_host_address;
}

bool WideUSBDevice::device_connected()
{
    return m_device_addr != 0;
}
