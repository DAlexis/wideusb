#ifndef DISCOVERY_HPP
#define DISCOVERY_HPP

#include "wideusb/communication/networking.hpp"
#include <list>


class DeviceDiscovery
{
public:
    /// @todo move this file to better location (communication/utils?)
    using OnDeviceDiscoveredCallback = std::function<void(Address)>;

    DeviceDiscovery(NetService& net_srv, Address local);

    void run(OnDeviceDiscoveredCallback callback = nullptr);
    const std::list<Address>& devices() const;

private:
    void discovery_socket_listener();

    Socket m_device_discovery_socket;
    OnDeviceDiscoveredCallback m_callback;
    std::list<Address> m_discovered;
};

#endif // DISCOVERY_HPP
