#include "modules/core.hpp"
#include "host-communication-interface.hpp"
/*
#include "rapidjson-config.h"
#include "rapidjson/document.h"

#include <string.h>

CoreModule::CoreModule() :
    Module("core")
{
    m_action_filter.add("status", [this](const rapidjson::Value&) -> ErrorCode { send_status(); return std::nullopt; } );
}

void CoreModule::receive_message(const rapidjson::Document& doc)
{
    if (!doc.HasMember("action"))
    {
        std::string msg = "action not specified";
        if (doc.HasMember("msg_id"))
        {
            msg += " for message id '";
            msg += doc["msg_id"].GetString();
            msg += "'";
        }
        assert_text(msg.c_str(), AssertLevel::Error, true);
    }
    if (strcmp(doc["action"].GetString(), "status") == 0)
    {
        send_status();
    }
}

void CoreModule::send_status()
{
    using namespace rapidjson;

    std::unique_ptr<Document> d(new Document);
    d->SetObject();
    auto & alloc = d->GetAllocator();

    add_module_field(*d);

    Value life_status("alive");
    d->AddMember("life_status", life_status, alloc);

    Value system_ticks(int(os::get_os_time()));
    d->AddMember("system_ticks", system_ticks, alloc);

    m_communicator->send_data(std::move(d));
}
*/
