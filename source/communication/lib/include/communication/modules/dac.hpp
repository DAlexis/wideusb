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

        struct InitRequest
        {
            constexpr static uint8_t magic = 1;
            const uint8_t magic_value = magic;

            constexpr static uint8_t single = 0;
            constexpr static uint8_t repeat = 1;
            constexpr static uint8_t continious = 2;

            uint32_t prescaler = 20;
            uint32_t period = 50;
            uint16_t buffer_size = 1000;
            uint8_t mode = single;
        };

        struct InitResponse
        {
            constexpr static uint8_t magic = 2;
            const uint8_t magic_value = magic;

            uint8_t success = 0;
            uint16_t actual_buffer_size = 0;
        };

        struct RunRequest
        {
            constexpr static uint8_t stop = 0;
            constexpr static uint8_t run = 1;

            constexpr static uint8_t magic = 3;
            const uint8_t magic_value = magic;

            uint8_t run_stop;
            uint16_t size_limit_notify = 100;
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
