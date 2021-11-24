#include "wideusb-pc/physical-layer-tcp-client.hpp"
#include <iostream>

using boost::asio::ip::tcp;

PhysicalLayerTcpClient::PhysicalLayerTcpClient(boost::asio::io_service& io_service, const std::string& addr, short port) :
    PhysicalLayerAsio(io_service),
    m_socket(io_service)
{
    tcp::resolver resolver(io_service);
    boost::asio::connect(m_socket, resolver.resolve(tcp::endpoint(tcp::v4(), port)));
    async_read();
}

void PhysicalLayerTcpClient::async_read()
{
    // TODO here check
    m_socket.async_read_some(
            boost::asio::buffer(m_input_buffer.data(), m_input_buffer.size() - 1),
            [this](const boost::system::error_code& error, std::size_t bytes_transferred) { on_data_read(error, bytes_transferred); }
        );
}

void PhysicalLayerTcpClient::async_send()
{
    /* This function may be called from SerialPortPhysicalLayer::send()
     * or from m_serial_port.async_write_some() callback. Both cases
     * are protected by m_write_strand, so it should not cause data race */
    if (m_output_ring_buffer.empty())
        return;

    m_data_to_send_now = m_output_ring_buffer.extract_buf();
    m_socket.async_write_some(
        boost::asio::buffer(m_data_to_send_now->data(), m_data_to_send_now->size()),
        boost::asio::bind_executor(
            m_write_strand,
            [this](const boost::system::error_code& error, std::size_t bytes_transferred) { on_data_sent(error, bytes_transferred); }
        )
    );
}
