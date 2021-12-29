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

WideusbDevice::WideusbDevice() :
    m_device_address(0x12345678),
    m_net_srv(
        std::make_shared<USBPhysicalLayer>(),
        std::make_shared<ChannelLayerBinary>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>()
    ),
    m_core(m_net_srv, m_device_address)
{
    m_core.add_module_factory(ids::monitor, [this](){ return create_monitor(); });
    m_core.add_module_factory(ids::gps, [this](){ return create_gps(); });
    m_core.add_module_factory(ids::dac, [this](){ return create_dac(); });
}

void WideusbDevice::run()
{
    for (;;)
    {
        m_net_srv.serve_sockets(std::chrono::steady_clock::now());
        m_core.tick();
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
