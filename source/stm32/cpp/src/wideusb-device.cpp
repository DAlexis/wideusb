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

#include "net-srv-runner.hpp"
#include "socket-queue-rtos.hpp"

#include "stm32f4xx_hal.h"

#include "devices/nrf24l01/nrf24l01-driver-hal.hpp"
#include "devices/nrf24l01/nrf24l01.hpp"

//#include "devices/sx1278/SX1278.h"
//#include "devices/sx1278/SX1278_hw.h"

#include "devices/sx1278/sx1278.hpp"
#include "devices/sx1278/sx1278-driver-hal.hpp"
#include "spi.h"

char buffer[512];

void blink()
{
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
}

WideusbDevice::WideusbDevice() :
    m_device_address(HAL_GetUIDw0() + HAL_GetUIDw1() + HAL_GetUIDw2()),
    m_net_srv(NetService::create(
        NetSrvRunner::create(),
//        nullptr,
        std::make_shared<QueueFactory>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>())
    ),
    m_core(m_net_srv, m_device_address)
{
    m_net_srv->add_interface(
                std::make_shared<NetworkInterface>(std::make_shared<USBPhysicalLayer>(m_common_async_worker), std::make_shared<ChannelLayerBinary>(), false));
    m_core.add_module_factory(ids::monitor, [this](){ return create_monitor(); });
    m_core.add_module_factory(ids::gps, [this](){ return create_gps(); });
    m_core.add_module_factory(ids::dac, [this](){ return create_dac(); });
}

//#define TEST_NRF
#define TEST_LORA

void WideusbDevice::run()
{
    os::delay(std::chrono::milliseconds((HAL_GetUIDw0() + HAL_GetUIDw1() + HAL_GetUIDw2()) % 1000));

#ifdef TEST_NRF
    std::shared_ptr<NRF24L01IODriverBase> nrf_driver = std::make_shared<NRF24L01IODriverHal>(
        GPIOPin(NRF24L01_CSN_GPIO_Port, NRF24L01_CSN_Pin),
        GPIOPin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin),
        GPIOPin(NRF24L01_IRQ_GPIO_Port, NRF24L01_IRQ_Pin),
        &hspi1
    );
    NRF24L01Config config;
    NRF24L01Manager nrf(nrf_driver, config);
    printf("NRF24L01 device created\n");
    nrf.print_status();
    printf("Status should be above\n");
    nrf.set_data_received_callback([](uint8_t ch, uint8_t* data){ printf("NRF data on channel %d: %s\n", ch, data); });
#endif

#ifdef TEST_LORA

    bool is_transmitter = false;


    auto sx1278driver = std::make_shared<SX1278DriverHAL>(
                GPIOPin(LORA_RESET_GPIO_Port, LORA_RESET_Pin),
                GPIOPin(LORA_INT_GPIO_Port, LORA_INT_Pin),
                GPIOPin(LORA_NSS_GPIO_Port, LORA_NSS_Pin),
                &hspi2);

    SX1278Device sx1278(sx1278driver);

    sx1278.init(434000000,
                SX1278Device::Power::POW_17DBM,
                SX1278Device::SpreadFactor::SF_7,
                SX1278Device::Bandwidth::BW_125KHZ,
                SX1278Device::CodingRate::CR_4_5,
                SX1278Device::CRC_Mode::enabled, 10);

/*
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
*/

    int ret;
    if (is_transmitter)
    {
//        ret = SX1278_LoRaEntryTx(&SX1278, 16, 2000);
        ret = sx1278.LoRaEntryTx(16, 2000);
    } else {
//        ret = SX1278_LoRaEntryRx(&SX1278, 16, 2000);
//        sx1278.status = SX1278Device::SX1278_STATUS::RX;
//        sx1278.packetLength = 16;
        ret = sx1278.LoRaEntryRx(16, 2000);
    }

    if (ret)
    {
        printf("LoRa initialization failed\n");
    }
#endif

    auto last_time_interrogate = std::chrono::steady_clock::now();
    auto last_time_send = std::chrono::steady_clock::now();
    int message = 0;
    for (;;)
    {
        //m_net_srv->serve_sockets(std::chrono::steady_clock::now());
        m_core.tick();
        auto time = std::chrono::steady_clock::now();

#ifdef TEST_NRF
        if (nrf.is_carrier_detected())
        {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
        }

        if (time - last_time_interrogate > 10ms)
        {
            nrf.tick();
            last_time_interrogate = time;
        }
#endif

        if (time - last_time_send > 2000ms)
        {
            last_time_send = time;
            printf("hi\n");
#ifdef TEST_NRF
            nrf.send(32, (uint8_t*)"test\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
            nrf.print_status();
#endif // TEST_NRF

#ifdef TEST_LORA
            if (is_transmitter)
            {
                printf("Sending package...\r\n");

                int message_length = sprintf(buffer, "Hello %d", message);
//                int ret = SX1278_LoRaEntryTx(&SX1278, message_length, 2000);
                int ret = sx1278.LoRaEntryTx(message_length, 2000);
                printf("Entry: %d\r\n", ret);

                printf("Sending %s\r\n", buffer);
//                ret = SX1278_LoRaTxPacket(&SX1278, (uint8_t*) buffer, message_length, 2000);
                ret = sx1278.LoRaTxPacket((uint8_t*) buffer, message_length, 2000);
                message += 1;

                printf("Transmission: %d\r\n", ret);
                printf("Package sent...\r\n");

            } else {
                printf("Receiving package...\r\n");

//                int ret = SX1278_LoRaRxPacket(&SX1278);
                int ret = sx1278.LoRaRxPacket();
                printf("Received: %d\r\n", ret);
                if (ret > 0) {
//                    SX1278_read(&SX1278, (uint8_t*) buffer, ret);
                    sx1278.read((uint8_t*) buffer, ret);
                    printf("Content (%d): %s\r\n", ret, buffer);
                }
                printf("Package received ...\r\n");
            }
#endif // TEST_LORA
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
