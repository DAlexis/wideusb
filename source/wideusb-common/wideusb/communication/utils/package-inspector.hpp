#ifndef PACKAGEINSPECTOR_HPP
#define PACKAGEINSPECTOR_HPP

#include <string>

#include "wideusb/communication/i-channel-layer.hpp"
#include "wideusb/communication/i-network-layer.hpp"
#include "wideusb/communication/i-transport-layer.hpp"

class PackageInspector
{
public:
    PackageInspector(std::shared_ptr<IChannelLayer> channel = nullptr,
                     std::shared_ptr<INetworkLayer> network = nullptr,
                     std::shared_ptr<ITransportLayer> transport = nullptr);

    std::string inspect_package(const PBuffer data, size_t max_buffer_len_to_print = 20);

private:
    std::shared_ptr<IChannelLayer> m_channel;
    std::shared_ptr<INetworkLayer> m_network;
    std::shared_ptr<ITransportLayer> m_transport;
};

#endif // PACKAGEINSPECTOR_HPP
