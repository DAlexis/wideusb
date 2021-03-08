#ifndef WIDEUSB_HPP_INCLUDED
#define WIDEUSB_HPP_INCLUDED

#include "serial-port-physical-layer.hpp"

#include "communication/networking.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#include <chrono>

#include <thread>
#include <string>
#include <memory>

class WideUSBDevice
{
public:
    WideUSBDevice(Address host_address, const std::string& port, int baudrate = 921600);

    void run_io_service();
    void test_socket();

    NetSevice& net_service();

    boost::asio::io_service& io_service();

    Address device_address();

private:
    class DeviceDiscoveryTask
    {
    public:
        using OnAddressUpdated = std::function<void(Address)>;
        DeviceDiscoveryTask(Address host_address, boost::asio::io_service& io_service, NetSevice& net_srv, OnAddressUpdated callback);

    private:

        void update_address();

        Socket discovery_sock;
        boost::posix_time::milliseconds interval;
        boost::asio::deadline_timer timer;
        OnAddressUpdated m_callback;
    };

    void post_serve_sockets();
    void test_monitor();

    uint32_t time_ms();

    void on_device_discovered(Address device_address);

    Address m_device_addr;
    Address m_host_address;

    boost::asio::io_service m_io_service;
    std::shared_ptr<SerialPortPhysicalLayer> m_physical_layer;
    NetSevice m_net_srv;

    std::unique_ptr<DeviceDiscoveryTask> m_device_discovery_task;


    //NetSevice m_net_service;
    std::chrono::time_point<std::chrono::steady_clock> m_creation;

};

#endif // WIDEUSB_HPP_INCLUDED
