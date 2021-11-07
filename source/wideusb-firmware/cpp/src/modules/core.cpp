#include "modules/core.hpp"
#include "modules/monitor-impl.hpp"
#include "modules/gps-impl.hpp"
#include "modules/dac-impl.hpp"

#include "host-communication/usb-physical-layer.hpp"
#include "wideusb-common/communication/binary/channel.hpp"
#include "wideusb-common/communication/binary/network.hpp"
#include "wideusb-common/communication/binary/transport.hpp"

#include "wideusb-common/communication/modules/ports.hpp"
#include "wideusb-common/communication/modules/core.hpp"
#include "wideusb-common/communication/modules/ids.hpp"

#include "os/cpp-freertos.hpp"


#include <stdio.h>

Core::Core() :
    m_net_srv(
        std::make_shared<USBPhysicalLayer>(),
        std::make_shared<ChannelLayerBinary>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>()
    ),
    m_device_discovery_sock(m_net_srv, m_device_address, ports::core::address_discovery),
    m_create_module_sock(m_net_srv, m_device_address, ports::core::create_module)
{
    m_device_discovery_sock.options().output_queue_limit = 5;
    m_device_discovery_sock.options().input_queue_limit = 5;
    m_device_discovery_sock.options().need_acknoledgement = false;
    m_device_discovery_sock.options().retransmitting_options.cycles_count = 1;
    m_device_discovery_sock.options().retransmitting_options.timeout = 0;
    m_device_discovery_sock.address_filter().listen_address(0x00000000, 0x00000000); // Any

}

bool Core::create_module(ModuleID id)
{  
    if (m_modules.find(id) != m_modules.end())
        return true;

    switch (id)
    {
    case ids::monitor:
        m_modules[id] = std::make_shared<MonitorImpl>(m_net_srv, m_device_address);
    break;
    case ids::gps:
        m_modules[id] = std::make_shared<GPSImpl>(m_net_srv, m_device_address);
    break;
    case ids::dac:
        m_modules[id] = std::make_shared<DACImpl>(m_net_srv, m_device_address);
    break;
    default:
        return false;
    }
    return true;
}

void Core::tick()
{
    m_net_srv.serve_sockets(os::get_os_time());
    poll_device_discovery();
    poll_create_module();
    for (auto &it : m_modules)
    {
        it.second->tick();
    }
}

void Core::poll_device_discovery()
{
    while (m_device_discovery_sock.has_data())
    {
        const Socket::IncomingMessage inc = *m_device_discovery_sock.get();

        core::discovery::Response response;
        PBuffer body = Buffer::create(sizeof(response), &response);
        m_device_discovery_sock.send(inc.sender, body);
    }
}

void Core::poll_create_module()
{
    while (m_create_module_sock.has_data())
    {
        const Socket::IncomingMessage inc = *m_create_module_sock.get();
        auto request = try_interpret_buffer_no_magic<core::create_module::Request>(inc.data);

        create_module(request->module_id);

        core::create_module::Response response;
        response.module_id = request->module_id;
        response.success = 1;
        PBuffer body = Buffer::serialize(response);
        m_create_module_sock.send(inc.sender, body);
    }
}
