#ifndef PYWIDEUSBDEVICE_HPP
#define PYWIDEUSBDEVICE_HPP

#include "wideusb/wideusb.hpp"
#include "wideusb/asio-utils.hpp"

#include <memory>

#include <iostream>

class PyWideUSBDevice
{
public:
    PyWideUSBDevice(uint64_t host_address, const std::string& port, int baudrate);
    ~PyWideUSBDevice() { std::cout << "Device destructed" << std::endl; }

    Address device_address();
    WideUSBHost::Device& device();
    void print_this() { std::cout << this << std::endl; }

private:
    std::unique_ptr<WideUSBHost::Device> m_device;
    std::unique_ptr<AsioServiceRunner> m_service_runner;
};

#endif // PYWIDEUSBDEVICE_HPP
