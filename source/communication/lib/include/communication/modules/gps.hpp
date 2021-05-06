#ifndef COMMUNICATION_GPS_HPP
#define COMMUNICATION_GPS_HPP

#include "communication/modules/ports.hpp"
#include "communication/modules/ids.hpp"
#include "communication/network-types.hpp"
#include <cstdint>

#pragma pack(push, 1)

namespace gps {

    constexpr static ModuleID id = ids::gps;

    namespace positioning {

        struct Request
        {
            char dummy = 'X';
        };

        struct Response
        {
            float latitude = 0;
            float longitude = 0;
            uint8_t has_pps = 0;
        };
    }

    namespace timestamping {

        constexpr static uint32_t port = ports::gps::timestamping;

        struct SubscribeRequest
        {
            constexpr static char remove = 0;
            constexpr static char add = 1;

            constexpr static uint8_t magic = 10;
            const uint8_t magic_value = magic;

            Address subscriber = 0;
            char action = 1; // 0 for remove
        };
        //static_assert (sizeof(SubscribeRequest) == 10, "bad size");

        struct SubscribeResponse
        {
            constexpr static uint8_t magic = 1;
            const uint8_t magic_value = magic;

            Address subscriber = 0;
            char success = 0;
        };

        struct TimestampingData
        {
            constexpr static uint8_t magic = 2;
            const uint8_t magic_value = magic;

            // @TODO Add position and timestamp data here
            bool has_pps = false;
        };

    }
}

#pragma pack(pop)

#endif // COMMUNICATION_GPS_HPP
