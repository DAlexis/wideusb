#ifndef WIDEUSB_HPP_INCLUDED
#define WIDEUSB_HPP_INCLUDED

#include "serial-port-physical-layer.hpp"

#include "communication/networking.hpp"
#include "asio-utils.hpp"
#include "host-modules/host-module.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#include <chrono>

#include <thread>

#include <set>
#include <string>
#include <memory>

class WideUSBDevice
{
public:
    using SingleTask = std::function<void(void)>;
    using OnDeviceDiscoveredCallback = std::function<void(void)>;
    WideUSBDevice(Address host_address, const std::string& port, OnDeviceDiscoveredCallback on_discovered = nullptr, int baudrate = 921600);

    void run_io_service();

    NetSevice& net_service();
    boost::asio::io_service& io_service();

    Address device_address();
    Address host_address();

    bool device_connected();
/*
    void connect_module(IModuleOnHost& module);
    void remove_module(IModuleOnHost& module);
*/
    void run_single(SingleTask task, size_t milliseconds = 0);

private:

    void post_serve_sockets();

    void run_device_discovery();
    void discovery_socket_listener();

    void create_module_socket_listener();

    uint32_t time_ms();

    // ASIO
    boost::asio::io_service m_io_service;

    // Networking
    std::shared_ptr<SerialPortPhysicalLayer> m_physical_layer;
    NetSevice m_net_srv;
    Task m_serve_sockets_task;

    Address m_device_addr = 0;
    Address m_host_address;

    // Sockets
    std::unique_ptr<Socket> m_device_discovery_socket;
    Socket m_create_module_socket;

    // Callbacks
    OnDeviceDiscoveredCallback m_on_discovered;

    // Other
    std::chrono::time_point<std::chrono::steady_clock> m_creation;
    //std::set<IModuleOnHost*> m_host_modules;

};

#endif // WIDEUSB_HPP_INCLUDED
