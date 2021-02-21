#include "wideusb.hpp"

#include <iostream>

using namespace boost::asio;


WideUSBDevice::WideUSBDevice(const std::string& port, int baudrate) :
    m_physical_layer(m_io_service, port, baudrate)
{

}

void WideUSBDevice::run_io_service()
{
    for(;;)
        m_io_service.run();
}
