#include "py-dac.hpp"
#include <iostream>

PyDAC::PyDAC(PyWideUSBDevice& device, Address custom_host_address, Address custom_device_address)
{
    Waiter<bool> waiter;
    m_dac.reset(new WideUSBHost::modules::DAC(device.device(), waiter.get_waiter_callback(), custom_host_address, custom_device_address));
    bool success = waiter.wait();
    if (!success)
        throw std::runtime_error("DAC module creation failed");
}

void PyDAC::init()
{
    Waiter<WideUSBHost::modules::DAC::InitResult> waiter;
    m_dac->init(false, waiter.get_waiter_callback());

    WideUSBHost::modules::DAC::InitResult result = waiter.wait();
    std::cout << "DAC init: " << result.success << "; actual_buffer_size: " << result.artual_buffer_size << std::endl;
}
