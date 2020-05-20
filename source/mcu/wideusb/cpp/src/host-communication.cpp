#include "host-communication.hpp"
#include "module.hpp"
#include "usbd_cdc_if.h"
#include "tools/ring-buffer-json.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


#include <string.h>

using namespace rapidjson;

static const char core_str[] = "core";
static const char debug[] = "debug";

HostCommunicator::HostCommunicator()
{
    m_thread.set_stack_size(1024);
}

void HostCommunicator::run_thread()
{
    m_thread.run();
}


void HostCommunicator::add_module(IModule* module)
{
    m_modules[module->name()] = module;
    module->connect_to_comminucator(this);
}

void HostCommunicator::send_data(const rapidjson::Document& doc)
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    buffer.Put('\r');
    buffer.Put('\n');
    // @Todo add check if USBD_BUSY and resend?
    CDC_Transmit_FS((uint8_t*)buffer.GetString(), buffer.GetSize());
}

void HostCommunicator::send_ack(const std::string& message_id)
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    Value module("communicator");
    d.AddMember("module", module, alloc);

    Value action("ack");
    d.AddMember("action", action, alloc);

    Value id(kStringType);
    id.SetString(StringRef(message_id.c_str()));
    d.AddMember("msg_id", id, alloc);

    send_data(d);
}

void HostCommunicator::parse_thread()
{
    std::optional<std::string> json;
    for (;;) {
        if (clear_by_timeout())
            continue;

        json = extract_json(&USBD_input_buffer);

        if (!json.has_value())
            return;

        parse_single_json(*json);
    }
}

void HostCommunicator::parse_single_json(const std::string& json)
{
    Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError())
    {
        return;
    }

    if (!doc.HasMember("module"))
    {
        return;
    }
    if (doc.HasMember("msg_id"))
    {
        send_ack(doc["msg_id"].GetString());
    }
}

bool HostCommunicator::clear_by_timeout()
{
    volatile uint32_t last_p_write = USBD_input_buffer.p_write;
    if (os::get_os_time() - USBD_input_buffer.last_time > incoming_timeout)
    {
        // Last incoming message was long time ago and was already parsed if it was consistent,
        // so all that not parsed since incoming_timeout ago is not valid.
        // We may clear buffer.
        USBD_input_buffer.p_read = last_p_write;
        return true;
    }
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
