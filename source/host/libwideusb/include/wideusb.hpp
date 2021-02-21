#ifndef WIDEUSB_HPP_INCLUDED
#define WIDEUSB_HPP_INCLUDED

#include "serial-port-physical-layer.hpp"
#include "communication/networking.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#include <thread>
#include <string>

class WideUSBDevice
{
public:
    WideUSBDevice(const std::string& port, int baudrate = 921600);
    void run_io_service();

private:
    boost::asio::io_service m_io_service;
    SerialPortPhysicalLayer m_physical_layer;


    //NetSevice m_net_service;
};

#endif // WIDEUSB_HPP_INCLUDED
