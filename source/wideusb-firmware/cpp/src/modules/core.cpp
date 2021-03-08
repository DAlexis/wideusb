#include "modules/core.hpp"
#include "host-communication-interface.hpp"

#include "host-communication/usb-physical-layer.hpp"
#include "communication/binary/channel.hpp"
#include "communication/binary/network.hpp"
#include "communication/binary/transport.hpp"

#include "communication/modules/ports.hpp"
#include "communication/modules/core.hpp"

#include "os/cpp-freertos.hpp"

#include <stdio.h>

Core::Core() :
    m_net_srv(
        std::make_shared<USBPhysicalLayer>(),
        std::make_shared<ChannelLayerBinary>(),
        std::make_shared<NetworkLayerBinary>(),
        std::make_shared<TransportLayerBinary>()
    ),
    m_device_discovery_sock(m_net_srv, m_device_address, ports::core::address_discovery)
{
    m_device_discovery_sock.options().output_queue_limit = 5;
    m_device_discovery_sock.options().input_queue_limit = 5;
    m_device_discovery_sock.options().need_acknoledgement = false;
    m_device_discovery_sock.options().retransmitting_options.cycles_count = 1;
    m_device_discovery_sock.options().retransmitting_options.timeout = 0;
    m_device_discovery_sock.address_filter().listen_address(0x00000000, 0x00000000); // Any
}

void Core::create_module(ModuleID id)
{
}

void Core::tick()
{
    m_net_srv.serve_sockets(os::get_os_time());
    serve_device_discovery();
}

void Core::serve_device_discovery()
{
    while (m_device_discovery_sock.has_data())
    {
        printf("Discovery address data!!\n");
        const Socket::IncomingMessage inc = *m_device_discovery_sock.get();

        DeviceDiscoveryResponse response;
        PBuffer body = Buffer::create(sizeof(response), &response);
        m_device_discovery_sock.send(inc.sender, body);
    }
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
