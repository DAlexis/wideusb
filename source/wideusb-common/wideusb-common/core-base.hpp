#ifndef COREBASE_HPP
#define COREBASE_HPP

#include "communication/networking.hpp"
#include "wideusb-common/module-base.hpp"

#include <vector>
#include <memory>
#include <map>

class CoreBase
{
public:
    using ModuleFactory = std::function<std::shared_ptr<IModule>()>;

    CoreBase(NetService& net_service, Address device_address);
    void tick();
    void add_module_factory(ModuleID id, ModuleFactory factory);

private:
    void sock_device_discovery_listener();
    void sock_create_module_listener();

    bool create_module(ModuleID id);

    Address m_device_address = 0x12345678;
    NetService& m_net_srv;

    std::shared_ptr<Socket> m_core_socket;
    std::map<ModuleID, std::shared_ptr<IModule>> m_modules;
    std::map<ModuleID, ModuleFactory> m_module_factories;
    Socket m_device_discovery_sock;
    Socket m_create_module_sock;
};


#endif // COREBASE_HPP
