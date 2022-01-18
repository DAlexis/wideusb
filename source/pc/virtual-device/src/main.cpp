#include "virtual-device.hpp"
#include <iostream>

#include <signal.h>

std::shared_ptr<IOServiceRunner> runner;


static void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        std::cout << "Interrupting virtual device" << std::endl;
        if (runner)
            runner->stop();
    }
}

int main()
{
    runner = IOServiceRunner::create();
    signal(SIGINT, signal_handler);
    std::cout << "Running virtual device" << std::endl;

    VirtualDevice dev(*runner, 12345);
    runner->join();
    std::cout << "Virtual device stopped" << std::endl;
    return 0;
}
