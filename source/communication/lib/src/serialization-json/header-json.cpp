#include "serialization-json/header-json.hpp"
#include "serialization-json/ring-buffer-json.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"

using namespace rapidjson;

std::optional<MessageHeader> HeaderParserJSON::deserialize(RingBuffer& buffer)
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
