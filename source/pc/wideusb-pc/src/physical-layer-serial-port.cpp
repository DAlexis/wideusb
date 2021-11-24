#include "wideusb-pc/physical-layer-serial-port.hpp"

#include <iostream>

PhysicalLayerSerialPort::PhysicalLayerSerialPort(
        boost::asio::io_service& io_service,
        const std::string& port,
        int baudrate) :
    PhysicalLayerAsio(io_service),
    m_serial_port(io_service)
{
    m_serial_port.open(port);
    m_serial_port.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
    async_read();
}

void PhysicalLayerSerialPort::async_read()
{
    std::cout << "async read" << std::endl;
    m_serial_port.async_read_some(
        boost::asio::buffer(m_input_buffer.data(), m_input_buffer.size() - 1),
        [this](const boost::system::error_code& error, std::size_t bytes_transferred) { on_data_read(error, bytes_transferred); }
    );
    std::cout << "async read exit" << std::endl;
}

void PhysicalLayerSerialPort::async_send()
{
    /* This function may be called from SerialPortPhysicalLayer::send()
     * or from m_serial_port.async_write_some() callback. Both cases
     * are protected by m_write_strand, so it should not cause data race */
    if (m_output_ring_buffer.empty())
        return;

    m_data_to_send_now = m_output_ring_buffer.extract_buf();
    m_serial_port.async_write_some(
        boost::asio::buffer(m_data_to_send_now->data(), m_data_to_send_now->size()),
        boost::asio::bind_executor(
            m_write_strand,
            [this](const boost::system::error_code& error, std::size_t bytes_transferred) { on_data_sent(error, bytes_transferred); }
        )
    );
}
