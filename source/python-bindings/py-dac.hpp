#ifndef PYDAC_HPP
#define PYDAC_HPP

#include "host-modules/dac.hpp"
#include "py-wideusb-device.hpp"

class PyDAC
{
public:
    PyDAC(PyWideUSBDevice& device, Address custom_host_address = 0, Address custom_device_address = 0);

    void init();

private:
    std::unique_ptr<WideUSBHost::modules::DAC> m_dac;
};


#endif // PYDAC_HPP
