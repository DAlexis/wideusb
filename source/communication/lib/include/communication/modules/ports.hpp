#ifndef PORTS_HPP
#define PORTS_HPP

#include <cstdint>

namespace ports
{
namespace core
{
    constexpr static uint32_t address_discovery = 2;
    constexpr static uint32_t create_module = 3;
}

namespace monitor
{
    constexpr static uint32_t status_update = 4;
}

namespace gps {
    constexpr static uint32_t position_update = 5;
}
};

#endif // PORTS_HPP
