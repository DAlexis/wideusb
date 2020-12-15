#include "json/msg-core-json.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"

#include "json/helpers-json.hpp"

using namespace rapidjson;


PBuffer JSONSerializer<StatusRequest>::serialize(const Message* msg) const
{
    const StatusRequest* request = static_cast<const StatusRequest*>(msg);
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    d.AddMember("do_blink", Value(request->do_blink), alloc);

    return buffer_from_document(d);
}

JSONDeserializer<StatusRequest>::JSONDeserializer(MessageDeserializer::Callback callback) :
    MessageDeserializer(callback)
{ }

bool JSONDeserializer<StatusRequest>::parse_impl(StatusRequest& target, const PBuffer buffer)
{
    Document d;
    d.Parse((const char*) buffer->data());

    if (!d.IsObject() || !d.HasMember("do_blink") || !d["do_blink"].IsBool())
        return false;

    m_message.do_blink = d["do_blink"].GetBool();
    return true;
}

PBuffer JSONSerializer<StatusResponse>::serialize(const Message* msg) const
{
    const StatusResponse* response = static_cast<const StatusResponse*>(msg);
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    Value life_status(response->alive);
    d.AddMember("alive", life_status, alloc);

    Value system_ticks(int(response->system_ticks));
    d.AddMember("system_ticks", system_ticks, alloc);

    Value free_mem(int(response->free_mem));
    d.AddMember("free_mem", free_mem, alloc);

    return buffer_from_document(d);
}

JSONDeserializer<StatusResponse>::JSONDeserializer(MessageDeserializer::Callback callback) :
    MessageDeserializer(callback)
{ }

bool JSONDeserializer<StatusResponse>::parse_impl(StatusResponse& target, PBuffer buffer)
{
    Document d;
    d.Parse((const char*) buffer->data());
    if (!d.HasMember("alive") || !d["alive"].IsBool())
        return false;

    if (!d.HasMember("system_ticks") || !d["system_ticks"].IsInt())
        return false;

    if (!d.HasMember("free_mem") || !d["free_mem"].IsInt())
        return false;

    m_message.alive = d["alive"].GetBool();
    m_message.system_ticks = d["system_ticks"].GetInt();
    m_message.free_mem = d["free_mem"].GetInt();
    return true;
}
