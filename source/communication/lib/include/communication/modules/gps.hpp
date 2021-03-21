#ifndef COMMUNICATION_GPS_HPP
#define COMMUNICATION_GPS_HPP

#include "communication/modules/ports.hpp"
#include "communication/modules/ids.hpp"
#include <cstdint>

namespace gps {

    constexpr static ModuleID id = ids::gps;

    namespace positioning {
        #pragma pack(push, 1)
        struct Request
        {
            char dummy = 'X';
        };
        #pragma pack(pop)


        #pragma pack(push, 1)
        struct Response
        {
            float latitude = 0;
            float longitude = 0;
            uint8_t has_pps = 0;
        };
        #pragma pack(pop)
    }
}

#endif // COMMUNICATION_GPS_HPP
