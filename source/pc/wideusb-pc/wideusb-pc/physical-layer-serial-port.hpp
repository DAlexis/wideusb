#ifndef SERIALPORTPHYSICALLAYER_HPP
#define SERIALPORTPHYSICALLAYER_HPP

#include "physical-layer-asio.hpp"

#include <boost/asio/serial_port.hpp>

class PhysicalLayerSerialPort : public PhysicalLayerAsio
{
public:
    PhysicalLayerSerialPort(
            std::shared_ptr<IOServiceRunner> io_service_runner,
            const std::string& port,
            int baudrate = 921600);

private:
    void async_read();
    void async_send() override;

    boost::asio::serial_port m_serial_port;
    PBuffer m_data_to_send_now;
};


#endif // SERIALPORTPHYSICALLAYER_HPP
