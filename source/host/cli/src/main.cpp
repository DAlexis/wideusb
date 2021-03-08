#include "wideusb.hpp"
#include "monitor.hpp"

#include <iostream>

int main()
{
    WideUSBDevice dev(0x12345678, 0x87654321, "/dev/ttyACM0");
    std::cout << "wideusb cli app stub" << std::endl;
    dev.test_socket();
    dev.run_io_service();
    return 0;
}
