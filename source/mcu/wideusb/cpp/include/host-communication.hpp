#ifndef HOST_COMMUNICATION_HPP_INCLUDED
#define HOST_COMMUNICATION_HPP_INCLUDED

#include "os/cpp-freertos.hpp"
#include "modules/core.hpp"
#include "host-communication-interface.hpp"

#include <string>
#include <vector>
#include <map>

class IModule;

class HostCommunicator : public IHostCommunicator
{
public:
    constexpr static os::Time_ms incoming_timeout = 20;

    HostCommunicator();

    void add_module(IModule* module) override;
    void send_data(const rapidjson::Document& doc) override;

    void run_thread();
    void send_ack(const std::string& message_id);

private:

    void parse_thread();
    void parse_single_json(const std::string& json);
    bool clear_by_timeout();

    std::map<std::string, IModule*> m_modules;

    os::Time_ms m_last_incoming = 0;
    os::TaskCycled m_thread{ [this](){ parse_thread(); }, "Usb_input_parsing"};
};

void debug_message(const std::string& message);

#endif // HOST_COMMUNICATION_HPP_INCLUDED
