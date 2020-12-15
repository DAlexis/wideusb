#include "json/header-json.hpp"
#include "json/ring-buffer-json.hpp"

#include "json/helpers-json.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"

using namespace rapidjson;

std::optional<MessageHeader> HeaderDeserializerJSON::deserialize(RingBuffer& buffer)
{
    auto result = extract_possible_json(&buffer);

    if (!result)
        return std::nullopt;

    Document doc;
    doc.Parse(result->c_str());

    if (!doc.HasMember("type") || !doc["type"].IsString())
        return std::nullopt;

    if (!doc.HasMember("size") || !doc["size"].IsUint())
        return std::nullopt;

    MessageHeader header;
    header.message_type = doc["type"].GetString();
    header.body_size_bytes = doc["size"].GetUint();
    return header;
}

PBuffer HeaderSerializerJSON::serialize(const MessageHeader& header)
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    Value type(kStringType);
    type.SetString(StringRef(header.message_type.c_str()), alloc);
    d.AddMember("type", type, alloc);
    d.AddMember("size", Value(header.body_size_bytes), alloc);

    return buffer_from_document(d);
}
