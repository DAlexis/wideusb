#include "devices/sx1278/sx1278-physical-layer.hpp"
#include "devices/sx1278/sx1278.hpp"
#include "os/cpp-freertos.hpp"
#include <stdio.h>

SX1278PhysicalLayer::SX1278PhysicalLayer(std::shared_ptr<SX1278DriverBase> driver, size_t ring_buffer_size) :
    m_sx1278(new SX1278Device(driver)),
    m_input_ring_buffer(ring_buffer_size),
    m_output_ring_buffer(ring_buffer_size),
    m_tx_buffer(message_len)
{
}

void SX1278PhysicalLayer::run()
{
    m_sx1278->init(434000000,
                  SX1278Device::Power::POW_17DBM,
                  SX1278Device::SpreadFactor::SF_7,
                  SX1278Device::Bandwidth::BW_125KHZ,
                  SX1278Device::CodingRate::CR_4_5,
                  SX1278Device::CRC_Mode::enabled, message_len);

    switch_to_rx();
    m_service_thread.run();
}

SerialReadAccessor& SX1278PhysicalLayer::incoming()
{
    return m_input_ring_buffer;
}

void SX1278PhysicalLayer::send(PBuffer data)
{
    m_output_ring_buffer.put(data);
}

void SX1278PhysicalLayer::set_on_data_callback(std::function<void(void)> callback)
{
    m_callback = callback;
}

void SX1278PhysicalLayer::tick()
{
    while(int size = m_sx1278->LoRaRxPacket())
    {
        m_input_ring_buffer.put(m_sx1278->get_rx_buffer().data(), size);
        printf("SX1278<= %d bytes\r\n", size);
    }
    if (!m_output_ring_buffer.empty())
    {
//      os::delay(300ms);
        auto now = std::chrono::steady_clock::now();
        if (now - m_last_send > 280ms)
        {
//        while (!m_output_ring_buffer.empty())
//        {
            int tx_size = std::min(int(m_output_ring_buffer.size()), message_len);
            printf("SX1278=> %d bytes\r\n", int(tx_size));
            m_output_ring_buffer.extract(m_tx_buffer.data(), tx_size);
            switch_to_tx();
            int ret = m_sx1278->LoRaTxPacket(m_tx_buffer.data(), tx_size, 2000);
            if (ret != 1)
            {
                printf("LoRaTxPacket() failed\n");
            }
//        }
            switch_to_rx();
            m_last_send = now;
        }
    }
}

void SX1278PhysicalLayer::service_loop()
{
    for (;;)
    {
        tick();
    }
}

void SX1278PhysicalLayer::switch_to_rx()
{
    int ret = m_sx1278->LoRaEntryRx(message_len, 2000);

    if (ret != 1)
    {
        printf("LoRaEntryRx() failed\n");
    }
}

void SX1278PhysicalLayer::switch_to_tx()
{
    int ret = m_sx1278->LoRaEntryTx(message_len, 2000);
    if (ret != 1)
    {
        printf("LoRaEntryTx() failed\r\n");
    }
}
