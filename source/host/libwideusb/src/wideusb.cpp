#include "wideusb.hpp"
#include "communication/binary/channel.hpp"
#include "communication/binary/network.hpp"
#include "communication/binary/transport.hpp"

#include <iostream>
#include <limits>

using namespace boost::asio;


WideUSBDevice::WideUSBDevice(const std::string& port, int baudrate) :
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
    m_creation(std::chrono::steady_clock::now()),
    m_monitor(m_net_srv, 0x87654321, 0x12345678, 1)
{

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
    m_sock.reset(new Socket(m_net_srv, 0x87654321, 0x12345678, 10));
    m_interval.reset(new boost::posix_time::seconds(1));
    m_timer.reset(new boost::asio::deadline_timer(m_io_service, *m_interval));
    m_timer->async_wait([this](const boost::system::error_code&){ test_monitor(); });
}

void WideUSBDevice::test_monitor()
{
    auto status = m_monitor.get_status();

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
        PBuffer data = m_sock->get();
        std::cout << data->data() << std::endl;
    }

    m_timer->expires_at(m_timer->expires_at() + *m_interval);
    m_timer->async_wait([this](const boost::system::error_code&){ test_monitor(); });
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
