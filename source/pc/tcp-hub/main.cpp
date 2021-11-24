#include "wideusb-pc/tcp-hub.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: tcp-hub <port>\n";
            return 1;
        }

        short port = std::atoi(argv[1]);

        boost::asio::io_service io_service;

        HubServer hub(io_service, port);

        std::cout << "Running tcp hub on port " << port << std::endl;
        io_service.run();
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }
}
