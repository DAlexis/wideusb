#ifndef SERIALPORTPHYSICALLAYER_HPP
#define SERIALPORTPHYSICALLAYER_HPP

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include "communication/i-physical-layer.hpp"

class SerialPortPhysicalLayer : public IPhysicalLayer
{
public:
    SerialPortPhysicalLayer(boost::asio::io_service& io_service, const std::string& port, int baudrate = 921600);
    SerialReadAccessor& incoming();
    void send(PBuffer data);

private:
    void async_read();
    void async_send();

    boost::asio::io_context::strand m_write_strand;
    boost::asio::serial_port m_serial_port;
    std::vector<uint8_t> m_input_buffer;
    RingBuffer m_input_ring_buffer;
    RingBuffer m_output_ring_buffer;
    PBuffer m_data_to_send_now;
};


#endif // SERIALPORTPHYSICALLAYER_HPP
