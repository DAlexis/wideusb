#include "modules/gps/nmea-receiver.hpp"
#include "os/cpp-freertos.hpp"

#include <algorithm>
#include <string>
#include <string.h>

static NMEAReceiver* interrupt_receiver = nullptr;

NMEAReceiver::NMEAReceiver(UART_HandleTypeDef* huart) :
    m_uart_handler(huart)
{
    interrupt_receiver = this;
    m_receiver_thread.run();
    m_monitor_thread.run();
    receive();
    receive();
}

const GPSData& NMEAReceiver::gps()
{
    return m_gps_data;
}

void NMEAReceiver::receiver_thread()
{
    for (;;)
    {
        SentenceBorders borders = *m_sentence_queue.pop_front();
        std::string line;
        if (borders.begin < borders.end)
        {
            line.insert(line.end(), m_buffer + borders.begin, m_buffer + borders.end);
        } else {
            line.insert(line.end(), m_buffer + borders.begin, m_buffer + buffer_size);
            line.insert(line.end(), m_buffer, m_buffer + borders.end);
        }
        m_gps_data.parse_line(line.c_str(), std::chrono::steady_clock::now());
        //printf("> %s\r\n", line.c_str());
        //printf(line.c_str());
    }
}

void NMEAReceiver::monitor_thread()
{
    int counter = 0;
    for (;;)
    {
        HAL_UART_StateTypeDef state = HAL_UART_GetState(m_uart_handler);
        if (state == HAL_UART_STATE_READY)
        {
            receive();
            counter++;
            printf("===> state fix\r\n");
        }
        os::delay(200ms);
    }
}

void NMEAReceiver::interrupt_uart_RX_callback(uint8_t* data, uint16_t size)
{
    // Moving to next block
    uint16_t readed_block = m_current_block;
    if (++m_current_block == blocks_count)
        m_current_block = 0;
    receive();

    // Scanning readed block for '$'
    for (uint16_t i = readed_block * block_size; i < (readed_block+1) * block_size; i++)
    {
        if (m_buffer[i] == '$')
        {
            SentenceBorders borders;
            borders.begin = m_last_sentence_end;
            borders.end = i;
            m_last_sentence_end = borders.end;
            if (borders.begin != borders.end)
                m_sentence_queue.push_back_from_ISR(borders);
        }
    }
}

void NMEAReceiver::interrupt_pps()
{
    m_gps_data.fit_to_pps(std::chrono::steady_clock::now());
}

void NMEAReceiver::receive()
{
    HAL_UART_Receive_IT(m_uart_handler, (uint8_t*) (m_buffer + m_current_block * block_size), block_size);
}

extern "C" {
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
        if (interrupt_receiver)
            interrupt_receiver->interrupt_uart_RX_callback(huart->pRxBuffPtr, huart->RxXferSize);
    }
}
