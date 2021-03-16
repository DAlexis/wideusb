#ifndef COMMUNICATION_MONITOR_HPP
#define COMMUNICATION_MONITOR_HPP

#include "communication/modules/ports.hpp"
#include "communication/modules/ids.hpp"
#include <cstdint>

namespace monitor {

    constexpr static ModuleID id = ids::core;

    namespace status {

        #pragma pack(push, 1)
        struct Request
        {
            char dummy = 'X';
        };
        #pragma pack(pop)


        #pragma pack(push, 1)
        struct Response
        {
            uint32_t heap_used = 0;
            uint32_t heap_total = 0;
        };
        #pragma pack(pop)
    }

}

#endif // COMMUNICATION_MONITOR_HPP
