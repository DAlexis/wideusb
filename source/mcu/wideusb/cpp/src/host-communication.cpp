#include "host-communication.hpp"
#include "usbd_cdc_if.h"
#include <string.h>

using namespace rapidjson;

static const char core_str[] = "core";
static const char debug[] = "debug";

void HostCommunicator::run_thread()
{
    m_thread.run();
}


void HostCommunicator::add_module(IModule* module)
{
    m_modules.push_back(module);
}

void HostCommunicator::parse_thread()
{


}

void HostCommunicator::check_timeout()
{
}

bool HostCommunicator::quick_check_buffer_ready()
{/*
    if (USBD_input_buffer.has_new_data == 0)
        return false;

    int braces_count = 0;
    for (uint32_t i = 0; i < USBD_input_buffer.length; i++)
    {
        if (USBD_input_buffer.ring_buffer[i] == '{')
            braces_count++;
        else if (USBD_input_buffer.ring_buffer[i] == '}')
            braces_count--;
        if (braces_count < 0)
            return false;
    }
    return braces_count == 0;*/
    return false;
}

void debug_message(const std::string& message)
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    Value module("core");
    d.AddMember("module", module, alloc);

    Value action("debug");
    d.AddMember("action", action, d.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    //cout << buffer.GetString() << endl;
}
