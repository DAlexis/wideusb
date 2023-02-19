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
#include "devices/sx1278/sx1278-physical-layer.hpp"
#include "spi.h"


//#define TEST_NRF
//#define TEST_LORA

char buffer[512];

void blink()
{
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
}

std::shared_ptr<SX1278PhysicalLayer> sx1278_phys_layer;

std::shared_ptr<os::Mutex> printf_mutex;

WideusbDevice::WideusbDevice() :
    m_device_address(get_unique_hw_id()),
    m_net_srv(NetService::create(
        NetSrvRunner::create(),
        std::make_shared<QueueFactory>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>(),
        nullptr,
        rand_modified)
    ),
    m_core(m_net_srv, m_device_address)
{
    srand(get_unique_hw_id());
    printf_mutex = std::make_shared<os::Mutex>();

    auto usb_interface = std::make_shared<NetworkInterface>(
                std::make_shared<USBPhysicalLayer>(m_common_async_worker),
                std::make_shared<ChannelLayerBinary>(),
                true);
    usb_interface->name = "usb";
    m_net_srv->add_interface(usb_interface);

#ifndef TEST_LORA
    auto sx1278driver = std::make_shared<SX1278DriverHAL>(
                GPIOPin(LORA_RESET_GPIO_Port, LORA_RESET_Pin),
                GPIOPin(LORA_INT_GPIO_Port, LORA_INT_Pin),
                GPIOPin(LORA_NSS_GPIO_Port, LORA_NSS_Pin),
                &hspi2);

    sx1278_phys_layer = std::make_shared<SX1278PhysicalLayer>(sx1278driver);
    sx1278_phys_layer->run();

    auto sx1278_interface = std::make_shared<NetworkInterface>(
                sx1278_phys_layer,
                std::make_shared<ChannelLayerBinary>(),
                true);

    sx1278_interface->name = "sx1278";
    m_net_srv->add_interface(sx1278_interface);
#endif // TEST_LORA

    m_core.add_module_factory(ids::monitor, [this](){ return create_monitor(); });
    m_core.add_module_factory(ids::gps, [this](){ return create_gps(); });
    m_core.add_module_factory(ids::dac, [this](){ return create_dac(); });
}



void transmit(SX1278Device& device, std::string str)
{
    int ret = device.LoRaEntryTx(str.size(), 2000);
    printf("Entry tx: %d, sending %s...\r\n", ret, str.c_str());

    ret = device.LoRaTxPacket((uint8_t*) str.c_str(), str.size()+1, 2000);
    printf("Transmission ret code: %d\r\n", ret);

    ret = device.LoRaEntryRx(16, 2000);
    printf("Switch to RX ret code: %d\r\n", ret);
}

std::string receive(SX1278Device& device)
{
    int size = device.LoRaRxPacket();
    if (size == 0)
        return "";
    printf("+ Received: %d\r\n", size);

    std::vector<uint8_t> buf = device.get_rx_buffer();
    buf[size-1] = 0;
    return (const char*)(buf.data());
}

void WideusbDevice::run()
{
    os::delay(std::chrono::milliseconds(get_unique_hw_id() % 1000));


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
    bool is_transmitter = true;

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

    int ret;
    if (is_transmitter)
    {
        ret = sx1278.LoRaEntryTx(16, 2000);
    } else {
        ret = sx1278.LoRaEntryRx(16, 2000);
    }

    if (ret == 0)
    {
        printf("LoRa initialization failed\r\n");
    }
#endif

    auto last_time_send = std::chrono::steady_clock::now();
    int message = 0;
    PBuffer buf = Buffer::create(16);
    for (;;)
    {
        //m_net_srv->serve_sockets(std::chrono::steady_clock::now());
//        sx1278_phys_layer->tick();
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

#ifdef TEST_LORA
        auto str = receive(sx1278);
        if (!str.empty())
        {
            printf("Received %s\r\n", str.c_str());
            if (!is_transmitter)
            {
                printf("Trying to answer\r\n");
                transmit(sx1278, "goodbuy");
            }
        }
#endif

        if (time - last_time_send > 2000ms)
        {
            last_time_send = time;
            printf("hi, im alive\r\n");

#ifdef TEST_NRF
            nrf.send(32, (uint8_t*)"test\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
            nrf.print_status();
#endif // TEST_NRF

#ifdef TEST_LORA
            if (is_transmitter)
            {
                printf("Sending package...\r\n");
                transmit(sx1278, "hello");
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

uint32_t WideusbDevice::get_unique_hw_id()
{
    return HAL_GetUIDw0() + HAL_GetUIDw1() + HAL_GetUIDw2();
}

uint32_t WideusbDevice::rand_modified()
{
    return (uint32_t(rand()) + os::get_ticks_count()) % RAND_MAX;
}
