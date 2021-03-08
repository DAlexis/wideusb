#include "wideusb.hpp"
#include "communication/binary/channel.hpp"
#include "communication/binary/network.hpp"
#include "communication/binary/transport.hpp"
#include "communication/modules/ports.hpp"
#include "communication/modules/core.hpp"

#include <iostream>
#include <limits>

using namespace boost::asio;


WideUSBDevice::WideUSBDevice(Address host_address, const std::string& port, int baudrate) :
    m_host_address(host_address),
    m_physical_layer(std::make_shared<SerialPortPhysicalLayer>(
        m_io_service,
        port,
        [this]() { post_serve_sockets(); },
        baudrate)),
    m_net_srv(
        m_physical_layer,
        std::make_shared<ChannelLayerBinary>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>()
    ),
    m_creation(std::chrono::steady_clock::now())
{
    m_device_discovery_task.reset(new DeviceDiscoveryTask(m_host_address, m_io_service, m_net_srv, [this](Address addr) { on_device_discovered(addr); } ));
}

void WideUSBDevice::run_io_service()
{
    for(;;)
    {
        m_io_service.run();
    }
}

void WideUSBDevice::test_socket()
{
    /*m_device_discovery_sock.reset(new Socket(m_net_srv, m_device_addr, 10));
    m_interval.reset(new boost::posix_time::seconds(1));
    m_timer.reset(new boost::asio::deadline_timer(m_io_service, *m_interval));
    m_timer->async_wait([this](const boost::system::error_code&){ test_monitor(); });*/
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

void WideUSBDevice::test_monitor()
{/*
    auto status = m_monitor.get_status();
    m_monitor.request_status();

    if (status)
    {
        std::cout << "Monitor status:" << std::endl;
        std::cout << *status << std::endl;
    } else {
        std::cout << "Monitor status not ready" << std::endl;
    }

    if (m_sock->has_data())
    {
        std::cout << "Socket has data:" << std::endl;
        PBuffer data = m_sock->get()->data;
        std::cout << data->data() << std::endl;
    }

    m_timer->expires_at(m_timer->expires_at() + *m_interval);
    m_timer->async_wait([this](const boost::system::error_code&){ test_monitor(); });*/
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

void WideUSBDevice::on_device_discovered(Address device_address)
{
    m_device_addr = device_address;
    m_device_discovery_task.reset();
}

WideUSBDevice::DeviceDiscoveryTask::DeviceDiscoveryTask(Address host_address, boost::asio::io_service& io_service, NetSevice& net_srv, OnAddressUpdated callback) :
    discovery_sock(net_srv, host_address, ports::core::address_discovery),
    interval(100),
    timer(io_service, interval),
    m_callback(callback)
{
    discovery_sock.options().output_queue_limit = 1;
    discovery_sock.options().input_queue_limit = 1;
    discovery_sock.options().need_acknoledgement = false;
    discovery_sock.options().retransmitting_options.cycles_count = 1;
    discovery_sock.options().retransmitting_options.timeout = 0;
    discovery_sock.address_filter().listen_address(0x00000000, 0x00000000); // Any
    discovery_sock.options().ttl = 1; // Do not retransmit over network
    update_address();
}

void WideUSBDevice::DeviceDiscoveryTask::update_address()
{
    if (discovery_sock.has_data())
    {
        Socket::IncomingMessage incoming = *discovery_sock.get();
        std::cout << "Received address!! " << incoming.sender << std::endl;
        if (m_callback)
            m_callback(incoming.sender);
    } else {
        DeviceDiscoveryRequest request;
        PBuffer body = Buffer::create(sizeof(request), &request);
        discovery_sock.send(0xFFFFFFFF, body);
        std::cout << "Sending address request..." << std::endl;
        timer.expires_at(timer.expires_at() + interval); // First run issue?
        timer.async_wait([this](const boost::system::error_code&){ update_address(); });
    }
}
