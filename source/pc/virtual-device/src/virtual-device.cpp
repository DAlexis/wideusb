#include "virtual-device.hpp"
#include "wideusb-pc/physical-layer-tcp-client.hpp"
#include "wideusb/communication/modules/ids.hpp"
#include "wideusb/communication/binary/channel.hpp"
#include "wideusb/communication/binary/network.hpp"
#include "wideusb/communication/binary/transport.hpp"
#include "modules/virtual-monitor-impl.hpp"


VirtualDevice::VirtualDevice(IOServiceRunner& io_service_runner, Address device_address) :
    m_io_service_runner(io_service_runner),
    m_physical(std::make_shared<PhysicalLayerTcpClient>(m_io_service_runner.io_service(), "127.0.0.1", 4321)),
    m_interface(std::make_shared<NetworkInterface>(m_physical,
                std::make_shared<ChannelLayerBinary>(),
                true)),
    m_net_srv(std::make_shared<NetworkLayerBinary>(),
              std::make_shared<TransportLayerBinary>(),
              [this]() { m_physical->post_serve_sockets(); }),
    m_core(m_net_srv, device_address),
    m_module_tick_task(m_io_service_runner.io_service(), 1, [this](){ m_core.tick(); return false; })
{
    m_net_srv.add_interface(m_interface);
    m_core.add_module_factory(ids::monitor, [this](){ return create_monitor(); } );
}

void VirtualDevice::run()
{
}

std::shared_ptr<IModule> VirtualDevice::create_monitor()
{
    return std::shared_ptr<MonitorBackImpl> (new MonitorBackImpl(m_net_srv, m_core.address()));
}
