#ifndef WIDEUSB_HPP_INCLUDED
#define WIDEUSB_HPP_INCLUDED

#include "serial-port-physical-layer.hpp"
#include "monitor.hpp"

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
    WideUSBDevice(Address device_addr, Address host_address, const std::string& port, int baudrate = 921600);
    void run_io_service();
    void test_socket();

private:
    void post_serve_sockets();
    void test_monitor();

    uint32_t time_ms();

    Address m_device_addr;
    Address m_host_address;

    boost::asio::io_service m_io_service;
    std::shared_ptr<SerialPortPhysicalLayer> m_physical_layer;
    NetSevice m_net_srv;


    std::unique_ptr<Socket> m_sock;
    std::unique_ptr<boost::posix_time::seconds> m_interval;  // 1 second
    std::unique_ptr<boost::asio::deadline_timer> m_timer;


    //NetSevice m_net_service;
    std::chrono::time_point<std::chrono::steady_clock> m_creation;

    Monitor m_monitor;
};

#endif // WIDEUSB_HPP_INCLUDED
