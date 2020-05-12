#ifndef HOST_COMMUNICATION_HPP_INCLUDED
#define HOST_COMMUNICATION_HPP_INCLUDED

#include "os/cpp-freertos.hpp"
#include "module.hpp"
#include "rapidjson-config.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


#include <string>
#include <vector>

class HostCommunicator
{
public:
    constexpr static size_t buffer_size = 1024;
    constexpr static os::Time_ms incoming_timeout = 2;

    void run_thread();
    void add_module(IModule* module);

private:
    void parse_thread();
    void check_timeout();

    // Check if input buffer ready to be parsed. If true, it MAY be consistent, if false, it is SURELY inconsistent
    bool quick_check_buffer_ready();

    uint8_t m_input_buffer[buffer_size];
    std::vector<IModule*> m_modules;

    os::Time_ms m_last_incoming = 0;
    os::TaskCycled m_thread{ [this](){ parse_thread(); }, "Usb_input_parsing"};
};

void debug_message(const std::string& message);

#endif // HOST_COMMUNICATION_HPP_INCLUDED
