#ifndef COMMUNICATION_MONITOR_HPP
#define COMMUNICATION_MONITOR_HPP

#include "wideusb/communication/modules/ports.hpp"
#include "wideusb/communication/modules/ids.hpp"
#include <cstdint>

namespace monitor {

    constexpr static ModuleID id = ids::monitor;

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

    namespace stdout_data {
        #pragma pack(push, 1)
        struct SubscribeRequest
        {
            constexpr static uint8_t unsubscribe = 0;
            constexpr static uint8_t subscribe = 1;

            uint8_t action = subscribe;
        };
        #pragma pack(pop)
    }

}

#endif // COMMUNICATION_MONITOR_HPP
