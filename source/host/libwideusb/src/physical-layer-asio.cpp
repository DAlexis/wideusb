#include "wideusb/physical-layer-asio.hpp"
#include "wideusb-common/communication/networking.hpp"

#ifdef INSPECT_PACKAGES
    #include <iostream>
#endif


PhysicalLayerAsio::PhysicalLayerAsio(boost::asio::io_service& io_service,
                                     size_t input_buffer_size,
                                     size_t input_ring_buffer_size,
                                     size_t output_ring_buffer_size) :
    m_io_service(io_service),
    m_write_strand(io_service),
    m_input_buffer(input_buffer_size),
    m_input_ring_buffer(input_ring_buffer_size),
    m_output_ring_buffer(output_ring_buffer_size),
    m_creation(std::chrono::steady_clock::now())
{
}

PhysicalLayerAsio::~PhysicalLayerAsio()
{
    std::cout << "dest" << std::endl;
}

SerialReadAccessor& PhysicalLayerAsio::incoming()
{
    return m_input_ring_buffer;
}

void PhysicalLayerAsio::send(PBuffer data)
{
#ifdef INSPECT_PACKAGES
    std::cout << " <= Outgoing data:" << data->size() << " bytes" << std::endl;
    std::cout << m_inspector.inspect_package(data) << std::endl;
#endif
    m_output_ring_buffer.put(data->data(), std::min(data->size(), m_output_ring_buffer.free_space()));
    boost::asio::post(m_write_strand, [this]() { async_send(); });
}

void PhysicalLayerAsio::on_network_service_connected(NetService& srv)
{
    m_net_service = &srv;
}

void PhysicalLayerAsio::post_serve_sockets()
{
    if (m_net_service)
        boost::asio::post(m_io_service, [this]() { m_net_service->serve_sockets(time_ms()); });
}

void PhysicalLayerAsio::on_data_read(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (error == boost::asio::error::operation_aborted)
    {
        std::cout << "ASIO Operation aborted!" << std::endl;
        return;
    }
    if (bytes_transferred != 0)
    {
        size_t data_size = std::min(m_input_ring_buffer.free_space(), bytes_transferred);
        m_input_buffer[bytes_transferred] = '\0';

#ifdef INSPECT_PACKAGES
        std::cout << " => Incoming data:" << bytes_transferred << " bytes" << std::endl;
        std::cout << m_inspector.inspect_package(Buffer::create(m_input_buffer.size(), m_input_buffer.data())) << std::endl;
#endif

        m_input_ring_buffer.put(m_input_buffer.data(), data_size);

        post_serve_sockets();
    }

    async_read();
}

uint32_t PhysicalLayerAsio::time_ms()
{
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_creation).count();
    return duration % std::numeric_limits<uint32_t>::max();
}

void PhysicalLayerAsio::on_data_sent(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (error == boost::asio::error::operation_aborted)
    {
        return;
    }
    //std::cout << bytes_transferred << " bytes sent to device" << std::endl;
    async_send();
}
