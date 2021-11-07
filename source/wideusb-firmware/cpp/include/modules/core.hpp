#ifndef MODULES_CORE_HPP_INCLUDED
#define MODULES_CORE_HPP_INCLUDED

#include "wideusb-common/module-base.hpp"
#include "wideusb-common/communication/networking.hpp"

#include <vector>
#include <memory>

class Core
{
public:
    Core();
    void tick();

private:
    bool create_module(ModuleID id);
    void poll_device_discovery();
    void poll_create_module();

    Address m_device_address = 0x12345678;
    NetService m_net_srv;

    std::shared_ptr<Socket> m_core_socket;
    std::map<ModuleID, std::shared_ptr<IModule>> m_modules;
    Socket m_device_discovery_sock;
    Socket m_create_module_sock;
};

/*
#include "os/cpp-freertos.hpp"
#include "module.hpp"

#include <queue>

class CoreModule : public Module
{
public:
    CoreModule();
    void receive_message(const rapidjson::Document& doc);

private:

    void send_status();
    std::queue<std::string> m_errors;

    //os::TaskCycled m_thread{ [this](){ thread_func(); }, "Core_module_thread" };
};
*/
#endif // MODULES_CORE_HPP_INCLUDED
