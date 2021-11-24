#ifndef CORE_HPP
#define CORE_HPP

#include "wideusb/communication/modules/ports.hpp"

#include <cstdint>

namespace core {

namespace discovery {

    constexpr static uint32_t port = ports::core::address_discovery;

    #pragma pack(push, 1)
    struct Request
    {
        char dummy = 'X';
    };
    #pragma pack(pop)


    #pragma pack(push, 1)
    struct Response
    {
        char dummy = 'X';
    };
    #pragma pack(pop)

}

namespace create_module {

    constexpr static uint32_t port = ports::core::create_module;

    #pragma pack(push, 1)
    struct Request
    {
        uint32_t module_id = 0;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct Response
    {
        uint32_t module_id = 0;
        uint8_t success = 0;
    };
    #pragma pack(pop)
}

}

#endif // CORE_HPP
