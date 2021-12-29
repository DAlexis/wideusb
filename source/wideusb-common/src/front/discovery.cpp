#include "wideusb/front/discovery.hpp"
#include "wideusb/communication/modules/core.hpp"

#include <iostream>

DeviceDiscovery::DeviceDiscovery(NetService& net_srv, Address local) :
    m_device_discovery_socket(
                net_srv, local, ports::core::address_discovery,
                [this](ISocketUserSide&){ discovery_socket_listener(); }
            )
{
    m_device_discovery_socket.options().retransmitting_options.cycles_count = 0;
    m_device_discovery_socket.options().retransmitting_options.timeout = 0ms;
    m_device_discovery_socket.options().ttl = 1; // Do not retransmit over network
    m_device_discovery_socket.address_filter().listen_address(0x00000000, 0x00000000); // Any
}

void DeviceDiscovery::run(OnDeviceDiscoveredCallback callback)
{
    m_callback = callback;
    core::discovery::Request request;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_device_discovery_socket.send(0xFFFFFFFF, body);
    std::cout << "Sending address request..." << std::endl;
}

const std::list<Address>& DeviceDiscovery::devices() const
{
    return m_discovered;
}

void DeviceDiscovery::discovery_socket_listener()
{
    Socket::IncomingMessage incoming = *m_device_discovery_socket.get();
    std::cout << "Device discovered!! " << incoming.sender << std::endl;
    m_discovered.push_back(incoming.sender);
    if (m_callback)
        m_callback(incoming.sender);
}
