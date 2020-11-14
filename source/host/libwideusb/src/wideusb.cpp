#include "wideusb.hpp"

#include <iostream>

WideUSBDevice::WideUSBDevice(const std::string& port, int baudrate)
{
    m_serial_port.open(port);
    m_serial_port.set_option(asio::serial_port_base::baud_rate(baudrate));
}
