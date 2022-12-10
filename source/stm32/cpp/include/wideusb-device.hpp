#ifndef WIDEUSBDEVICE_HPP
#define WIDEUSBDEVICE_HPP

#include "wideusb/device-core.hpp"
#include "os/async-worker.hpp"

class WideusbDevice
{
public:
    WideusbDevice();

    void run();

private:
    static uint32_t get_unique_hw_id();
    static uint32_t rand_modified();

    std::shared_ptr<IModule> create_monitor();
    std::shared_ptr<IModule> create_gps();
    std::shared_ptr<IModule> create_dac();

    Address m_device_address;

    AsyncWorker m_common_async_worker{2048, 10, "com_async"};
    NetService::ptr m_net_srv;
    DeviceCore m_core;

};

#endif // WIDEUSBDEVICE_HPP
