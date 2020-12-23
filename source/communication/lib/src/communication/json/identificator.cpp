#include "communication/json/identificator.hpp"

#include "communication/json/rapidjson-config.h"
#include "rapidjson/document.h"

#include "communication/json/helpers-json.hpp"

using namespace rapidjson;

void IdentificatorJSON::put_message_id(PBuffer buf, MessageId id)
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    d.AddMember("msg_id", Value(id), alloc);
    *buf << *buffer_from_document(d);
}

std::optional<MessageId> IdentificatorJSON::get_message_id(PBuffer buf, size_t& pos)
{

    return std::nullopt;
}
