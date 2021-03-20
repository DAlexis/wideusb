#ifndef HOSTMODULE_HPP
#define HOSTMODULE_HPP

#include <communication/networking.hpp>

#include <memory>
#include <functional>

namespace WideUSBHost
{

class Device;

namespace modules
{

class IModule
{
public:
    virtual ~IModule() = default;
};

class ModuleBase : public IModule
{
public:
    using OnModuleCreatedCallback = std::function<void(bool)>;
    ModuleBase(Device& host_connection_service, uint32_t module_id, Address device_address, Address host_address, OnModuleCreatedCallback on_created);
    ~ModuleBase();

protected:
    void create_module();

    Device& m_host_connection_service;
    Address m_device_address;
    Address m_host_address;

    uint32_t m_module_id;
    OnModuleCreatedCallback m_on_created;

private:
    void create_module_socket_listener();
    std::unique_ptr<Socket> m_create_module_socket;
};

}

}

#endif // HOSTMODULE_HPP
