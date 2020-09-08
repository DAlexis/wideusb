#include "host-communication.hpp"
#include "module.hpp"
#include "usbd_cdc_if.h"
#include "tools/ring-buffer-json.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


#include <string.h>
#include <mutex>

using namespace rapidjson;

static const char core_str[] = "core";
static const char debug[] = "debug";

HostCommunicator::HostCommunicator()
{
}

void HostCommunicator::run_thread()
{
    m_input_parsing_thread.run();
    m_output_sending_thread.run();
}

void HostCommunicator::set_core_module(CoreModule* core_module)
{
    m_core_module = core_module;
}

void HostCommunicator::add_module(IModule* module)
{
    m_modules[module->name()] = module;
    module->connect_to_comminucator(this);
}

void HostCommunicator::send_data(std::unique_ptr<rapidjson::Document> doc)
{
    std::unique_lock<os::Mutex> lock(m_output_queue_mutex);
    m_output_messages.emplace(std::move(doc));
    lock.unlock();
    m_output_sending_thread.notify_give();
}

void HostCommunicator::send_ack(const std::string& message_id)
{
    std::unique_ptr<Document> d(new Document);
    d->SetObject();
    auto & alloc = d->GetAllocator();

    Value module("communicator");
    d->AddMember("module", module, alloc);

    Value action("ack");
    d->AddMember("action", action, alloc);

    Value id(kStringType);
    id.SetString(message_id.c_str(), message_id.size(), alloc);
    d->AddMember("msg_id", id, alloc);

    send_data(std::move(d));
}

void HostCommunicator::input_parsing_thread_func()
{
    std::optional<std::string> json;
    for (;;) {
        if (clear_by_timeout())
            continue;

        json = extract_json(&USBD_input_buffer);

        if (!json.has_value())
            continue;

        parse_single_json(*json);
    }
}

void HostCommunicator::output_messages_sending_thread_func()
{
    for (;;)
    {
        os::Thread::notify_take();

        while (!m_output_messages.empty())
        {
            std::unique_ptr<Document> doc(std::move(m_output_messages.front()));
            std::unique_lock<os::Mutex> lock(m_output_queue_mutex);
            m_output_messages.pop();
            lock.unlock();

            StringBuffer buffer;
            Writer<StringBuffer> writer(buffer);
            doc->Accept(writer);
            buffer.Put('\r');
            buffer.Put('\n');
            // @Todo add check if USBD_BUSY and resend?
            CDC_Transmit_FS((uint8_t*)buffer.GetString(), buffer.GetSize());
        }
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
    auto it = m_modules.find(doc["module"].GetString());
    if (it == m_modules.end())
    {
        if (m_core_module == nullptr)
            return;
        std::string error_text = "Module '";
        error_text += doc["module"].GetString();
        error_text += "' not found";
        m_core_module->assert_text(error_text.c_str(), AssertLevel::Error, true);
        return;
    }
    it->second->receive_message(doc);
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
