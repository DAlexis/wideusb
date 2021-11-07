#ifndef PHYSICALLAYERASIO_HPP
#define PHYSICALLAYERASIO_HPP

#include <boost/asio.hpp>
#include "wideusb-common/communication/i-physical-layer.hpp"

#define INSPECT_PACKAGES

#ifdef INSPECT_PACKAGES
#include "wideusb-common/communication/utils/package-inspector.hpp"
#endif

class PhysicalLayerAsio : public IPhysicalLayer
{
public:
    PhysicalLayerAsio(boost::asio::io_service& io_service,
                      size_t input_buffer_size = 10,
                      size_t input_ring_buffer_size = 10000,
                      size_t output_ring_buffer_size = 10000);

    SerialReadAccessor& incoming() override;
    void send(PBuffer data) override;
    void on_network_service_connected(NetService& srv) override;

protected:
    virtual void async_read() = 0;
    virtual void async_send() = 0;

    virtual void on_data_read(const boost::system::error_code& error, std::size_t bytes_transferred);
    virtual void on_data_sent(const boost::system::error_code& error, std::size_t bytes_transferred);

    uint32_t time_ms();

    NetService* m_net_service = nullptr;

    boost::asio::io_service& m_io_service;
    boost::asio::io_context::strand m_write_strand;
    std::vector<uint8_t> m_input_buffer;
    RingBuffer m_input_ring_buffer;
    RingBuffer m_output_ring_buffer;

    std::chrono::time_point<std::chrono::steady_clock> m_creation;

#ifdef INSPECT_PACKAGES
    PackageInspector m_inspector;
#endif
};


#endif // PHYSICALLAYERASIO_HPP
