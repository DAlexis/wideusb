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
    auto driver = NRF24L01IODriverImpl::get();
    NRF24L01Config config;
    NRF24L01Manager nrf(driver, config);
    printf("NRF24L01 device created\n");
    nrf.print_status();
    printf("Status should be above\n");
    nrf.set_data_received_callback([](uint8_t ch, uint8_t* data){ printf("NRF data on channel %d: %s\n", ch, data); });

    auto last_time_interrogate = std::chrono::steady_clock::now();
    auto last_time_send = std::chrono::steady_clock::now();
    for (;;)
    {
        m_net_srv.serve_sockets(std::chrono::steady_clock::now());
        m_core.tick();
        if (nrf.is_carrier_detected())
        {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
        }
        auto time = std::chrono::steady_clock::now();
        if (time - last_time_interrogate > 10ms)
        {
            nrf.tick();
            last_time_interrogate = time;
        }

        if (time - last_time_send > 1s)
        {
            last_time_send = time;
            nrf.send(32, (uint8_t*)"test\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
            printf("Sending\n");
            nrf.print_status();
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
