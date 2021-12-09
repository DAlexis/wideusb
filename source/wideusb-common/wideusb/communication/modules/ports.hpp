#ifndef PORTS_HPP
#define PORTS_HPP

#include "wideusb/communication/network-types.hpp"

namespace ports
{
namespace core
{
    constexpr static Port address_discovery = 2;
    constexpr static Port create_module = 3;
}

namespace monitor
{
    constexpr static Port status_update = 4;
    constexpr static Port stdout_data = 40;
}

namespace gps {
    constexpr static Port positioning = 5;
    constexpr static Port timestamping = 6;
}

namespace dac {
    constexpr static Port setup = 10;
    constexpr static Port data = 11;
}
};

#endif // PORTS_HPP
