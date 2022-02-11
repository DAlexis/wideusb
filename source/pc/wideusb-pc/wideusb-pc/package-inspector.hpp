#ifndef PACKAGEINSPECTOR_HPP
#define PACKAGEINSPECTOR_HPP

#include <string>

#include "wideusb/communication/i-channel-layer.hpp"
#include "wideusb/communication/i-network-layer.hpp"
#include "wideusb/communication/i-transport-layer.hpp"
#include "wideusb/communication/i-package-inspector.hpp"

#include <chrono>

class PackageInspector : public IPackageInspector
{
public:
    PackageInspector(std::shared_ptr<IChannelLayer> channel = nullptr,
                     std::shared_ptr<INetworkLayer> network = nullptr,
                     std::shared_ptr<ITransportLayer> transport = nullptr);

    void inspect_package(const PBuffer data, const std::string& context_msg) override;

private:
    std::shared_ptr<IChannelLayer> m_channel;
    std::shared_ptr<INetworkLayer> m_network;
    std::shared_ptr<ITransportLayer> m_transport;
    size_t m_max_buffer_len_to_print = 20;
    std::chrono::steady_clock::time_point m_created;
};

#endif // PACKAGEINSPECTOR_HPP
