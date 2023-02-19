#include "wideusb-pc/physical-layer-asio.hpp"
#include "wideusb/communication/networking.hpp"

#include <iostream>

PhysicalLayerAsio::PhysicalLayerAsio(std::shared_ptr<IOServiceRunner> io_service_runner,
                                     std::shared_ptr<IPackageInspector> package_inspector,
                                     size_t input_buffer_size,
                                     size_t input_ring_buffer_size,
                                     size_t output_ring_buffer_size) :
    m_io_service_runner(io_service_runner),
    m_write_strand(io_service_runner->io_service()),
    m_input_buffer(input_buffer_size),
    m_input_ring_buffer(input_ring_buffer_size),
    m_output_ring_buffer(output_ring_buffer_size),
    m_package_inspector(package_inspector),
    m_creation(std::chrono::steady_clock::now())
{
}

PhysicalLayerAsio::~PhysicalLayerAsio()
{
}

SerialReadAccessor& PhysicalLayerAsio::incoming()
{
    return m_input_ring_buffer;
}

void PhysicalLayerAsio::send(PBuffer data)
{
    if (m_package_inspector)
    {
        m_package_inspector->inspect_package(data, "<= Outgoint package");
    }

    m_output_ring_buffer.put(data->data(), std::min(data->size(), m_output_ring_buffer.free_space()));
    boost::asio::post(m_write_strand, [this]() { async_send(); });
}

void PhysicalLayerAsio::set_on_data_callback(std::function<void(void)> callback)
{
    m_on_data_received_callback = callback;
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
//        m_input_buffer[bytes_transferred] = '\0';


        m_input_ring_buffer.put(m_input_buffer.data(), data_size);

        if (m_on_data_received_callback)
            m_on_data_received_callback();
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
