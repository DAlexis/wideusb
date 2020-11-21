#include "serialization-json/json-helpers.hpp"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

PBuffer buffer_from_document(const rapidjson::Document& doc)
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    buffer.Put('\r');
    buffer.Put('\n');
    return Buffer::create(buffer.GetSize() + 1, (uint8_t*) buffer.GetString());
}
