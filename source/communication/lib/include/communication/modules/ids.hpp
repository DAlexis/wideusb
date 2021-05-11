#ifndef IDS_HPP
#define IDS_HPP

#include "communication/modules/types.hpp"
#include <cstdint>

namespace ids
{
constexpr static ModuleID core = 1;
constexpr static ModuleID monitor = 2;
constexpr static ModuleID gps = 3;
constexpr static ModuleID dac = 4;
}

#endif // IDS_HPP
