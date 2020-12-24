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
    *buf << BufferAccessor(buffer_from_document(d));
}

std::optional<MessageId> IdentificatorJSON::get_message_id(SerialReadAccessor& accessor)
{
    auto json = extract_possible_json(accessor);
    if (!json.has_value())
        return std::nullopt;

    Document d;
    d.Parse(json->c_str());
    if (!d.HasMember("msg_id") || !d["msg_id"].IsUint())
        return std::nullopt;

    return d["msg_id"].GetUint();
}
