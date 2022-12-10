#include "wideusb/device-core.hpp"
#include "wideusb/back/base-back.hpp"
#include "wideusb/communication/modules/core.hpp"
#include "wideusb/communication/modules/ids.hpp"

DeviceCore::DeviceCore(NetService::ptr net_service, Address device_address) :
    m_device_address(device_address),
    m_net_srv(net_service),
    m_device_discovery_sock(m_net_srv, m_device_address, ports::core::address_discovery, [this](ISocketUserSide&) { sock_device_discovery_listener(); }),
    m_create_module_sock(m_net_srv, m_device_address, ports::core::create_module, [this](ISocketUserSide&) { sock_create_module_listener(); })
{
    m_device_discovery_sock.options().output_queue_limit = 5;
    m_device_discovery_sock.options().input_queue_limit = 5;
    m_device_discovery_sock.options().need_acknoledgement = false;
    m_device_discovery_sock.options().retransmitting_options.cycles_count = 1;
    m_device_discovery_sock.options().retransmitting_options.timeout = 0ms;
    m_device_discovery_sock.address_filter().listen_address(0xFFFFFFFF, 0xFFFFFFFF); // Only 0xFFFFFFFF for device discovery
}

bool DeviceCore::create_module(ModuleID id)
{
    if (m_modules.find(id) != m_modules.end())
        return true;

    auto it = m_module_factories.find(id);
    if (it == m_module_factories.end())
    {
        return false;
    }

    m_modules[id] = it->second();

    return true;
}

void DeviceCore::tick()
{
    for (auto &it : m_modules)
    {
        it.second->tick();
    }
}

void DeviceCore::add_module_factory(ModuleID id, ModuleFactory factory)
{
    m_module_factories[id] = factory;
}

Address DeviceCore::address()
{
    return m_device_address;
}

void DeviceCore::sock_device_discovery_listener()
{
    while (m_device_discovery_sock.has_incoming())
    {
        const Socket::IncomingMessage inc = *m_device_discovery_sock.get_incoming();
        printf("[DeviceCore] Device discovery request received\r\n");
        core::discovery::Response response;
        PBuffer body = Buffer::create(sizeof(response), &response);
        m_device_discovery_sock.send(inc.sender, body);
    }
}

void DeviceCore::sock_create_module_listener()
{
    while (m_create_module_sock.has_incoming())
    {
        const Socket::IncomingMessage inc = *m_create_module_sock.get_incoming();
        auto request = try_interpret_buffer_no_magic<core::create_module::Request>(inc.data);

        bool success = create_module(request->module_id);

        core::create_module::Response response;
        response.module_id = request->module_id;
        response.success = success ? 1 : 0;
        PBuffer body = Buffer::serialize(response);
        m_create_module_sock.send(inc.sender, body);
    }
}
