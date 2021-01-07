#ifndef NETWORKINGINTERFACES_HPP
#define NETWORKINGINTERFACES_HPP

#include "buffer.hpp"
#include "communication/network-base.hpp"

#include <vector>
#include <optional>

struct SegmentHeader
{
    uint32_t package_id;
    uint8_t options;
    Port port;

    struct Options {
        constexpr static uint8_t none = 0;
        constexpr static uint8_t need_ack = 0x1;
        constexpr static uint8_t is_ack = 0x2;
    };
};

#endif // NETWORKINGINTERFACES_HPP
