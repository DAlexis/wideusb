#ifndef HOST_COMMUNICATION_HPP_INCLUDED
#define HOST_COMMUNICATION_HPP_INCLUDED

#include "os/cpp-freertos.hpp"
#include "modules/core.hpp"
#include "host-communication-interface.hpp"

#include <string>
#include <vector>
#include <map>
#include <queue>

class IModule;
class CoreModule;

class HostCommunicator : public IHostCommunicator
{
public:
    constexpr static os::Time_ms incoming_timeout = 20;

    HostCommunicator();

    void set_core_module(CoreModule* core_module);
    void add_module(IModule* module) override;
    void send_data(std::unique_ptr<rapidjson::Document> doc) override;

    void run_thread();
private:

    void send_ack(const std::string& message_id);

    void input_parsing_thread_func();
    void output_messages_sending_thread_func();
    void parse_single_json(const std::string& json);
    bool clear_by_timeout();

    std::map<std::string, IModule*> m_modules;
    CoreModule *m_core_module = nullptr;

    os::Time_ms m_last_incoming = 0;
    os::Thread m_input_parsing_thread { [this](){ input_parsing_thread_func(); }, "Usb_input_parsing", 1024 };
    os::Thread m_output_sending_thread { [this](){ output_messages_sending_thread_func(); }, "Usb_output_sending", 1024 };

    std::queue<std::unique_ptr<rapidjson::Document>> m_output_messages;
    os::Mutex m_output_queue_mutex;

    ObjectFilter m_modules_filter{"module"};
};

#endif // HOST_COMMUNICATION_HPP_INCLUDED
