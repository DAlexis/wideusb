#ifndef VIRTUALDEVICE_HPP
#define VIRTUALDEVICE_HPP

#include "wideusb/device-core.hpp"
#include "wideusb-pc/physical-layer-asio.hpp"
#include "wideusb-pc/asio-utils.hpp"

class VirtualDevice
{
public:
    VirtualDevice(IOServiceRunner& io_service_runner, Address device_address);

    void run();

private:
    std::shared_ptr<IModule> create_monitor();

    IOServiceRunner& m_io_service_runner;

    std::shared_ptr<PhysicalLayerAsio> m_physical;
    std::shared_ptr<NetworkInterface> m_interface;
    NetService m_net_srv;
    DeviceCore m_core;
//    Task m_module_tick_task;
};

#endif // VIRTUALDEVICE_HPP
