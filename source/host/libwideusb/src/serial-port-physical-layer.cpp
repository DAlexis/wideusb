#include "serial-port-physical-layer.hpp"

#include <iostream>

SerialPortPhysicalLayer::SerialPortPhysicalLayer(
        boost::asio::io_service& io_service,
        const std::string& port,
        OnIncominDataCallback incoming_callback,
        int baudrate) :
    m_write_strand(io_service), m_serial_port(io_service), m_incoming_callback(incoming_callback), m_input_buffer(100), m_input_ring_buffer(10000), m_output_ring_buffer(10000)
{
    m_serial_port.open(port);
    m_serial_port.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
    async_read();
}

void SerialPortPhysicalLayer::async_read()
{
    m_serial_port.async_read_some(
        boost::asio::buffer(m_input_buffer.data(), m_input_buffer.size() - 1),
        [this](const boost::system::error_code& error, std::size_t bytes_transferred)
        {
            if (error == boost::asio::error::operation_aborted)
            {
                return;
            }
            if (bytes_transferred != 0)
            {
                size_t data_size = std::min(m_input_ring_buffer.free_space(), bytes_transferred);
                m_input_buffer[bytes_transferred] = '\0';
                //std::cout << "receiverd bytes: " << bytes_transferred << ": " << m_input_buffer.data() << std::endl;
                m_input_ring_buffer.put(m_input_buffer.data(), data_size);


                if (m_incoming_callback)
                    m_incoming_callback();
            }

            async_read();
        }
    );
}

void SerialPortPhysicalLayer::async_send()
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
            [this](const boost::system::error_code& error, std::size_t bytes_transferred)
            {
                if (error == boost::asio::error::operation_aborted)
                {
                    return;
                }
                std::cout << bytes_transferred << " bytes sent to device" << std::endl;
                async_send();
            }
        )
    );
}

SerialReadAccessor& SerialPortPhysicalLayer::incoming()
{
    return m_input_ring_buffer;
}

void SerialPortPhysicalLayer::send(PBuffer data)
{
    m_output_ring_buffer.put(data->data(), std::min(data->size(), m_output_ring_buffer.free_space()));
    boost::asio::post(m_write_strand, [this]() { async_send(); });
}
