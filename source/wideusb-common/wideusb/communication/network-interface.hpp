#ifndef NETWORKINTERFACE_HPP
#define NETWORKINTERFACE_HPP

#include "wideusb/communication/i-physical-layer.hpp"
#include "wideusb/communication/i-channel-layer.hpp"

#include <memory>

struct NetworkInterface
{
    NetworkInterface(std::shared_ptr<IPhysicalLayer> physical, std::shared_ptr<IChannelLayer> channel, bool retransmission_to_interface) :
        physical(physical), channel(channel), enable_retransmission_here(retransmission_to_interface)
    {}

    std::shared_ptr<IPhysicalLayer> physical;
    std::shared_ptr<IChannelLayer> channel;
    bool enable_retransmission_here = true;
    bool enable_retransmission_back = false;

    const char* name = "default";
};


#endif // NETWORKINTERFACE_HPP
