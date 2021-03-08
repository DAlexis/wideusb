#ifndef MODULES_CORE_HPP_INCLUDED
#define MODULES_CORE_HPP_INCLUDED

#include "module-base.hpp"
#include "communication/networking.hpp"

#include <vector>
#include <memory>

class Core
{
public:
    Core();
    void create_module(ModuleID id);

    void tick();

private:
    void serve_device_discovery();

    Address m_device_address = 0x12345678;
    NetSevice m_net_srv;

    std::shared_ptr<Socket> m_core_socket;
    std::vector<IModule> m_modules;
    Socket m_device_discovery_sock;

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
