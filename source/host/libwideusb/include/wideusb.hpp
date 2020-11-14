#ifndef WIDEUSB_HPP_INCLUDED
#define WIDEUSB_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#include <thread>
#include <string>

class WideUSBDevice
{
public:
    WideUSBDevice(const std::string& port, int baudrate = 921600);

private:
    boost::asio::io_service m_io_service;
    boost::asio::serial_port m_serial_port{m_io_service};
};

#endif // WIDEUSB_HPP_INCLUDED
