#ifndef WIDEUSBDEVICE_HPP
#define WIDEUSBDEVICE_HPP

#include "wideusb-common/core-base.hpp"

class WideusbDevice
{
public:
    WideusbDevice();

    void run();

private:
    std::shared_ptr<IModule> create_monitor();
    std::shared_ptr<IModule> create_gps();
    std::shared_ptr<IModule> create_dac();

    Address m_device_address;

    NetService m_net_srv;
    CoreBase m_core;

};

#endif // WIDEUSBDEVICE_HPP