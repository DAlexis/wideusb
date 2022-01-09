#include "wideusb-device.hpp"

#include "modules/monitor-impl.hpp"
#include "modules/gps-impl.hpp"
#include "modules/dac-impl.hpp"

#include "host-communication/usb-physical-layer.hpp"
#include "wideusb/communication/binary/channel.hpp"
#include "wideusb/communication/binary/network.hpp"
#include "wideusb/communication/binary/transport.hpp"

#include "wideusb/communication/modules/ids.hpp"

#include "os/cpp-freertos.hpp"

#include "stm32f4xx_hal.h"

#include "devices/nrf24l01-driver-impl.hpp"
#include "devices/nrf24l01.hpp"

#include "devices/sx1278/SX1278.h"
#include "devices/sx1278/SX1278_hw.h"
#include "spi.h"

char buffer[512];

WideusbDevice::WideusbDevice() :
    m_device_address(HAL_GetUIDw0() + HAL_GetUIDw1() + HAL_GetUIDw2()),
    m_net_srv(
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>()
    ),
    m_core(m_net_srv, m_device_address)
{
    m_net_srv.add_interface(
                std::make_shared<NetworkInterface>(std::make_shared<USBPhysicalLayer>(), std::make_shared<ChannelLayerBinary>(), false));
    m_core.add_module_factory(ids::monitor, [this](){ return create_monitor(); });
    m_core.add_module_factory(ids::gps, [this](){ return create_gps(); });
    m_core.add_module_factory(ids::dac, [this](){ return create_dac(); });
}

void WideusbDevice::run()
{
    os::delay(std::chrono::milliseconds((HAL_GetUIDw0() + HAL_GetUIDw1() + HAL_GetUIDw2()) % 1000));
    /*auto driver = NRF24L01IODriverImpl::get();
    NRF24L01Config config;
    NRF24L01Manager nrf(driver, config);
    printf("NRF24L01 device created\n");
    nrf.print_status();
    printf("Status should be above\n");
    nrf.set_data_received_callback([](uint8_t ch, uint8_t* data){ printf("NRF data on channel %d: %s\n", ch, data); });
*/

    SX1278_hw_t sx1278_config;
    sx1278_config.spi = &hspi2;
    sx1278_config.dio0.port = LORA_INT_GPIO_Port;
    sx1278_config.dio0.pin = LORA_INT_Pin;

    sx1278_config.nss.port = LORA_NSS_GPIO_Port;
    sx1278_config.nss.pin = LORA_NSS_Pin;

    sx1278_config.reset.port = LORA_RESET_GPIO_Port;
    sx1278_config.reset.pin = LORA_RESET_Pin;

    SX1278_t SX1278;
    SX1278.hw = &sx1278_config;
    SX1278_init(&SX1278, 434000000, SX1278_POWER_17DBM, SX1278_LORA_SF_7, SX1278_LORA_BW_125KHZ, SX1278_LORA_CR_4_5, SX1278_LORA_CRC_EN, 10);

    bool is_transmitter = true;

    if (is_transmitter)
    {
        int ret = SX1278_LoRaEntryTx(&SX1278, 16, 2000);
    } else {
        int ret = SX1278_LoRaEntryRx(&SX1278, 16, 2000);
    }

    auto last_time_interrogate = std::chrono::steady_clock::now();
    auto last_time_send = std::chrono::steady_clock::now();
    int message = 0;
    for (;;)
    {
        m_net_srv.serve_sockets(std::chrono::steady_clock::now());
        m_core.tick();

  /*      if (nrf.is_carrier_detected())
        {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
        }*/
        auto time = std::chrono::steady_clock::now();
        /*if (time - last_time_interrogate > 10ms)
        {
            nrf.tick();
            last_time_interrogate = time;
        }*/



        if (time - last_time_send > 100ms)
        {
            last_time_send = time;

            if (is_transmitter)
            {
                printf("Sending package...\r\n");

                int message_length = sprintf(buffer, "Hello %d", message);
                int ret = SX1278_LoRaEntryTx(&SX1278, message_length, 2000);
                printf("Entry: %d\r\n", ret);

                printf("Sending %s\r\n", buffer);
                ret = SX1278_LoRaTxPacket(&SX1278, (uint8_t*) buffer,
                        message_length, 2000);
                message += 1;

                printf("Transmission: %d\r\n", ret);
                printf("Package sent...\r\n");

            } else {
                printf("Receiving package...\r\n");

                int ret = SX1278_LoRaRxPacket(&SX1278);
                printf("Received: %d\r\n", ret);
                if (ret > 0) {
                    SX1278_read(&SX1278, (uint8_t*) buffer, ret);
                    printf("Content (%d): %s\r\n", ret, buffer);
                }
                printf("Package received ...\r\n");

    /*            nrf.send(32, (uint8_t*)"test\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");

                nrf.print_status();
                */
                //sx1278->transmit((uint8_t*) "LoRa test\0", 10);
                //printf("Sending\n");
            }
        }
    }
}

std::shared_ptr<IModule> WideusbDevice::create_monitor()
{
    return std::make_shared<MonitorImpl>(m_net_srv, m_device_address);
}

std::shared_ptr<IModule> WideusbDevice::create_gps()
{
    return std::make_shared<GPSImpl>(m_net_srv, m_device_address);
}

std::shared_ptr<IModule> WideusbDevice::create_dac()
{
    return std::make_shared<DACImpl>(m_net_srv, m_device_address);
}
