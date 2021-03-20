#include "py-wideusb-device.hpp"

#include <ctime>
#include <cstdlib>

PyWideUSBDevice::PyWideUSBDevice(uint64_t host_address, const std::string& port, int baudrate)
{
    srand(time(NULL));
    Waiter<void> waiter;
    m_device.reset(new WideUSBHost::Device(host_address, port, waiter.get_waiter_callback(), baudrate));
    m_service_runner.reset(new AsioServiceRunner(m_device->io_service()));
    m_service_runner->run_thread();
    waiter.wait();
}

Address PyWideUSBDevice::device_address()
{
    return m_device->device_address();
}

WideUSBHost::Device& PyWideUSBDevice::device()
{
    return *m_device;
}
