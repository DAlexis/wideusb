#ifndef COMMUNICATION_DAC_HPP
#define COMMUNICATION_DAC_HPP

#include "communication/modules/ports.hpp"
#include "communication/modules/ids.hpp"
#include "communication/network-types.hpp"
#include <cstdint>

#pragma pack(push, 1)

namespace dac {

    constexpr static ModuleID id = ids::dac;

    namespace setup {
        constexpr static uint32_t port = ports::dac::setup;

        struct Timings
        {
            uint32_t prescaler = 20;
            uint32_t period = 50;
        };

        struct InitContinious
        {
            constexpr static uint8_t magic = 100;
            const uint8_t magic_value = magic;

            Timings timings;
            uint16_t buffer_size = 1000;
            uint16_t chunk_size = 100;
            uint32_t notify_when_left = 300;
        };

        struct InitSample
        {
            constexpr static uint8_t magic = 101;
            const uint8_t magic_value = magic;

            Timings timings;
            uint16_t buffer_size = 1000;
            uint8_t autorepeat = 0;
        };

        struct InitResponse
        {
            constexpr static uint8_t magic = 2;
            const uint8_t magic_value = magic;

            uint8_t error_code = 0;
        };

        struct RunRequest
        {
            constexpr static uint8_t stop = 0;
            constexpr static uint8_t run = 1;

            constexpr static uint8_t magic = 3;
            const uint8_t magic_value = magic;

            uint8_t run_stop;
            Address status_reports_receiver;
        };
    }
    namespace data {
        constexpr static uint32_t port = ports::dac::data;

        struct BufferIsShortNotification
        {
            constexpr static uint8_t magic = 10;
            const uint8_t magic_value = magic;
            uint16_t buffer_size = 0;
        };
    }
}

#pragma pack(pop)

#endif // COMMUNICATION_DAC_HPP
