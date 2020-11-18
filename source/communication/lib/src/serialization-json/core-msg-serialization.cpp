#include "serialization-json/core-msg-serialization.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

StatusRequestSerializer::StatusRequestSerializer(const StatusRequest& request) :
    m_request(request)
{
}

PBuffer StatusRequestSerializer::serialize()
{
    PBuffer result;
    return result;
}

bool StatusRequestDeserializer::parse_impl(StatusRequest& target, RingBuffer& buffer)
{
    return false;
}


StatusResponseSerializer::StatusResponseSerializer(const StatusResponse& response) :
    m_response(response)
{
}

PBuffer StatusResponseSerializer::serialize()
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    Value life_status(m_response.alive);
    d.AddMember("alive", life_status, alloc);

    Value system_ticks(int(m_response.system_ticks));
    d.AddMember("system_ticks", system_ticks, alloc);

    Value free_mem(int(m_response.free_mem));
    d.AddMember("system_ticks", free_mem, alloc);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
    buffer.Put('\r');
    buffer.Put('\n');

    return Buffer::create(buffer.GetSize(), (uint8_t*) buffer.GetString());
}

bool StatusResponseDeserializer::parse_impl(StatusResponse& target, RingBuffer& buffer)
{

}
