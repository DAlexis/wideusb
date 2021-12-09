#ifndef OSTYPES_HPP
#define OSTYPES_HPP

#include <functional>
#include <stdint.h>

using TaskFunction = std::function<void(void)>;
using Time_ms = uint32_t;
using Priority = int;
using Ticks = uint32_t;
using Handle = void*;

#endif // OSTYPES_HPP
