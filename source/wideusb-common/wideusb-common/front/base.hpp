#ifndef FRONT_BASE_HPP
#define FRONT_BASE_HPP

#include <communication/networking.hpp>

#include <memory>
#include <functional>

class ModuleFrontBase
{
public:
    using OnModuleCreatedCallback = std::function<void(bool)>;
    ModuleFrontBase(NetSevice& host_connection_service, uint32_t module_id, Address device_address, Address host_address, OnModuleCreatedCallback on_created);
    ~ModuleFrontBase();

protected:
    void create_module();

    NetSevice& m_host_connection_service;
    Address m_device_address;
    Address m_host_address;

    uint32_t m_module_id;
    OnModuleCreatedCallback m_on_created;

private:
    void create_module_socket_listener();
    std::unique_ptr<Socket> m_create_module_socket;
};


#endif // FRONT_BASE_HPP
