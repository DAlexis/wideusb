#ifndef VIRTUALDEVICE_HPP
#define VIRTUALDEVICE_HPP

#include "wideusb-common/core-base.hpp"
#include "wideusb/physical-layer-asio.hpp"
#include "wideusb/asio-utils.hpp"

class VirtualDevice
{
public:
    VirtualDevice(IOServiceRunner& io_service_runner, Address device_address);

    void run();

private:
    std::shared_ptr<IModule> create_monitor();

    IOServiceRunner& m_io_service_runner;

    std::shared_ptr<PhysicalLayerAsio> m_physical;
    NetService m_net_srv;
    DeviceCore m_core;
    Task m_module_tick_task;
};

#endif // VIRTUALDEVICE_HPP
