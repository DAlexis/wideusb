#include "wideusb.hpp"
#include "monitor.hpp"

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>


int main()
{
    srand(time(NULL));
    WideUSBDevice dev(0x87654321, "/dev/ttyACM0");
    std::cout << "wideusb cli app stub" << std::endl;
    dev.test_socket();
    dev.run_io_service();
    return 0;
}
