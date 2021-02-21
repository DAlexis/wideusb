#include "wideusb.hpp"

#include <iostream>

int main()
{
    WideUSBDevice dev("/dev/ttyACM0");
    std::cout << "wideusb cli app stub" << std::endl;
    dev.run_io_service();
    return 0;
}
