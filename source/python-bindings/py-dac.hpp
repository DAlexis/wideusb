#ifndef PYDAC_HPP
#define PYDAC_HPP

#include "py-wideusb-device.hpp"
#include "wideusb-common/front/dac-front.hpp"

class PyDAC
{
public:
    PyDAC(PyWideUSBDevice& device, Address custom_host_address = 0, Address custom_device_address = 0);

    size_t max_buffer_size();

    void init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat);
    void run();
    void send_data(const std::vector<float>& data);

private:
    std::unique_ptr<DACFront> m_dac;
    std::vector<float>* m_data;
};


#endif // PYDAC_HPP
