#include "modules/core.hpp"
#include "host-communication-interface.hpp"

#include "host-communication/usb-physical-layer.hpp"
#include "communication/binary/channel.hpp"
#include "communication/binary/network.hpp"
#include "communication/binary/transport.hpp"

Core::Core()
{
    m_service = std::make_shared<NetSevice>(
        std::make_shared<USBPhysicalLayer>(),
        std::make_shared<ChannelLayerBinary>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>()
    );
    m_core_socket = std::make_shared<Socket>(*m_service, m_device_address, 2);
}

void Core::create_module(ModuleID id)
{
}

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
