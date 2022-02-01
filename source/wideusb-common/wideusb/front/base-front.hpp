#ifndef FRONT_BASE_HPP
#define FRONT_BASE_HPP

#include "wideusb/communication/networking.hpp"
#include "wideusb/utils/utilities.hpp"

#include <memory>
#include <functional>

class ModuleFrontBase
{
public:
    using OnModuleCreatedCallbackEntry = CallbackEntry<bool>;

    using OnModuleCreatedCallback = std::function<void(bool)>;
    ModuleFrontBase(NetService::ptr host_connection_service, uint32_t module_id, Address device_address, Address host_address, OnModuleCreatedCallbackEntry on_created);
    ~ModuleFrontBase();

protected:
    void create_module();

    NetService::ptr m_host_connection_service;
    Address m_device_address;
    Address m_host_address;

    uint32_t m_module_id;
    OnModuleCreatedCallbackEntry m_on_created;

private:
    void create_module_socket_listener();
    std::unique_ptr<Socket> m_create_module_socket;
};


#endif // FRONT_BASE_HPP
