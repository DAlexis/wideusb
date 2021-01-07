#include "communication/json/identificator.hpp"

#include "communication/json/rapidjson-config.h"
#include "rapidjson/document.h"

#include "communication/json/helpers-json.hpp"

using namespace rapidjson;

bool IdentificatorJSON::put_message_id(SerialWriteAccessor& write_accessor, const MessageHeader& header)
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    d.AddMember("id", Value(header.id), alloc);
    d.AddMember("key", Value(header.key), alloc);

    BufferAccessor message_buffer_accessor(buffer_from_document(d));
    if (!write_accessor.will_fit(message_buffer_accessor.size()))
        return false;

    write_accessor.put(message_buffer_accessor);
    return true;
}

std::optional<MessageHeader> IdentificatorJSON::get_message_id(SerialReadAccessor& accessor)
{
    auto json = extract_possible_json(accessor);
    if (!json.has_value())
        return std::nullopt;

    Document d;
    d.Parse(json->c_str());
    if (!d.HasMember("id") || !d["id"].IsUint())
        return std::nullopt;

    if (!d.HasMember("key") || !d["key"].IsUint())
        return std::nullopt;

    return MessageHeader(d["id"].GetUint(), d["key"].GetUint());
}
