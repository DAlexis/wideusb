#ifndef NMEA_RECEIVER_HPP_INCLUDED
#define NMEA_RECEIVER_HPP_INCLUDED

#include "modules/gps/nmea-parser.hpp"
#include "os/cpp-freertos.hpp"
#include "usart.h"
#include <stdint.h>

class NMEAReceiver
{
public:
    NMEAReceiver(UART_HandleTypeDef* huart);

    const GPSData& gps();

    void interrupt_uart_RX_callback(uint8_t* data, uint16_t size);
    void interrupt_pps();

private:
    struct SentenceBorders
    {
        uint16_t begin = 0, end = 0;
    };

    constexpr static uint16_t block_size = 16;
    constexpr static uint16_t blocks_count = 16;
    constexpr static uint16_t buffer_size = block_size * blocks_count;

    void receive();
    void receiver_thread();
    void monitor_thread();

    GPSData m_gps_data;

    char m_buffer[buffer_size];
    uint16_t m_current_block = 0;

    uint16_t m_last_sentence_end = 0;
    uint16_t m_sentence_begin = 0;

    uint16_t m_receiver_buffer_offset = 0;
    uint16_t m_result_buffer_size = 0;

    UART_HandleTypeDef* m_uart_handler;

    os::Queue<SentenceBorders> m_sentence_queue;

    //os::Thread m_receiver_thread{[this](){ receiver_thread(); }, "NMEA_receiver", 1024};
    os::Thread m_receiver_thread{[this](){ receiver_thread(); }, "NMEA_parser", 1024};
    os::Thread m_monitor_thread{[this](){ monitor_thread(); }, "monitior", 128};
};

#endif // NMEA_RECEIVER_HPP_INCLUDED
