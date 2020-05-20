#ifndef MODULES_CORE_HPP_INCLUDED
#define MODULES_CORE_HPP_INCLUDED

#include "os/cpp-freertos.hpp"
#include "module.hpp"

#include <queue>

class CoreModule : public Module
{
public:
    const char* name() override;
    void receive_message(const rapidjson::Document& doc) override;

private:

    std::queue<std::string> m_errors;

    //os::TaskCycled m_thread{ [this](){ thread_func(); }, "Core_module_thread" };
};

#endif // MODULES_CORE_HPP_INCLUDED
