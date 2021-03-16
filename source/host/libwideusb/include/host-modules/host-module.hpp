#ifndef HOSTMODULE_HPP
#define HOSTMODULE_HPP

#include <communication/networking.hpp>

#include <memory>
#include <functional>

class WideUSBDevice;

class IModuleOnHost
{
public:
    virtual void on_device_ready() = 0;
    virtual ~IModuleOnHost() = default;
};

class ModuleOnHostBase : public IModuleOnHost
{
public:
    using OnModuleCreatedCallback = std::function<void(bool)>;
    ModuleOnHostBase(WideUSBDevice& host_connection_service, uint32_t module_id, Address device_address, Address host_address, OnModuleCreatedCallback on_created);
    ~ModuleOnHostBase();

protected:
    void assert_device_ready();
    void create_module();

    WideUSBDevice& m_host_connection_service;
    Address m_device_address;
    Address m_host_address;

    uint32_t m_module_id;
    OnModuleCreatedCallback m_on_created;

private:
    void create_module_socket_listener();
    std::unique_ptr<Socket> m_create_module_socket;
};

#endif // HOSTMODULE_HPP
