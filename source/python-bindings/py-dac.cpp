#include "py-dac.hpp"
#include <iostream>

using namespace WideUSBHost::modules;

PyDAC::PyDAC(PyWideUSBDevice& device, Address custom_host_address, Address custom_device_address)
{
    Waiter<bool> waiter;
    m_dac.reset(new DAC(device.device(), waiter.get_waiter_callback(), custom_host_address, custom_device_address));
    bool success = waiter.wait();
    if (!success)
        throw std::runtime_error("DAC module creation failed");
}

void PyDAC::init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat)
{
    Waiter<int> waiter;
    m_dac->init_sample(buffer_size, prescaler, period, repeat, waiter.get_waiter_callback());
    int result = waiter.wait();
    if (result != 0)
        throw std::runtime_error("DAC initialization failed with code " + std::to_string(result));
}

void PyDAC::send_data(const std::vector<float>& data)
{
    Waiter<size_t> waiter;
    m_dac->send_data(data, waiter.get_waiter_callback());
    size_t actual_sent = waiter.wait();
    if (actual_sent != data.size()*2)
        throw std::runtime_error("DAC data send was incomplete: " + std::to_string(actual_sent) + " btes actual sent of " + std::to_string(data.size()*2));
}

void PyDAC::run()
{
    Waiter<bool> waiter;
    m_dac->run(waiter.get_waiter_callback());
    if (!waiter.wait())
        throw std::runtime_error("DAC failed to run");
}

size_t PyDAC::max_buffer_size()
{
    return 1000;
}
