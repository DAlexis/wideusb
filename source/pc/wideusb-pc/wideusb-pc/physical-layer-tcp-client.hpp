#ifndef TCPPHYSICALLAYER_HPP
#define TCPPHYSICALLAYER_HPP

#include <boost/asio.hpp>
#include "physical-layer-asio.hpp"

class PhysicalLayerTcpClient : public PhysicalLayerAsio
{
public:
    PhysicalLayerTcpClient(std::shared_ptr<IOServiceRunner> io_service_runner, const std::string& addr, short port);

private:
    void async_read() override;
    void async_send() override;

    boost::asio::ip::tcp::socket m_socket;

    PBuffer m_data_to_send_now;
};

#endif // TCPPHYSICALLAYER_HPP
