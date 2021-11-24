#ifndef TCPPHYSICALLAYER_HPP
#define TCPPHYSICALLAYER_HPP

#include <boost/asio.hpp>
#include "physical-layer-asio.hpp"

#define INSPECT_PACKAGES

#ifdef INSPECT_PACKAGES
#include "wideusb/communication/utils/package-inspector.hpp"
#endif

class PhysicalLayerTcpClient : public PhysicalLayerAsio
{
public:
    PhysicalLayerTcpClient(boost::asio::io_service& io_service, const std::string& addr, short port);

private:
    void async_read() override;
    void async_send() override;

    boost::asio::ip::tcp::socket m_socket;

    PBuffer m_data_to_send_now;
};

#endif // TCPPHYSICALLAYER_HPP
