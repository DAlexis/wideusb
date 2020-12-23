#include "communication/json/helpers-json.hpp"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

PBuffer buffer_from_document(const rapidjson::Document& doc)
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    //buffer.Put('\r');
    //buffer.Put('\n');
    return Buffer::create(buffer.GetSize(), (uint8_t*) buffer.GetString());
}

std::optional<std::string> extract_possible_json(ISerialReadAccessor& ring_buffer)
{
    /* Buffer content may be extended during this function call,
     * when interrupt may be called.
     */

    if (ring_buffer.empty())
        return std::nullopt; // buffer is empty

    // Detecting possibly valid JSON string
    // Detecting '{'

    size_t buffer_data_size = ring_buffer.size();
    bool success = false;
    size_t i = 0;
    for (; i != buffer_data_size; i++)
    {
        if (ring_buffer[i] == '{')
        {
            success = true;
            break;
        }
    }
    ring_buffer.skip(i);

    if (!success)
    {
        // We reached p_write, but there are no '{', only trash
        // Wait for next try
        return std::nullopt;
    }

    uint32_t possible_json_begin = 0;

    buffer_data_size = ring_buffer.size();

    // OK, possible JSON beginning, lets find possible end
    int braces_count = 0;
    success = false;
    for(i = 0; i != buffer_data_size; i++)
    {
        char ch = ring_buffer[i];

        switch(ch)
        {
        case '{': braces_count += 1; break;
        case '}': braces_count -= 1; break;
        }

        if (braces_count == 0)
        {
            success = true;
            break;
        } else if (braces_count < 0)
        {
            success = false;
            // Next read attempt after the '}' brace
            ring_buffer.skip(i);
            // skip next???
            break;
        }
    }

    if (!success)
        return std::nullopt;

    uint32_t possible_json_end = i+1;

    size_t possible_json_size = i+1;

    std::string result(possible_json_size, 'X');

    ring_buffer.get((uint8_t*) &result[0], possible_json_size);
/*
    // removing CR, LF, \0 from buffer
    while (ring_buffer_data_size(ring_buffer) >= 1)
    {
        uint8_t c = *ring_buffer_at(ring_buffer, 0);
        if (c != '\r' && c != '\n' && c != '\0')
            break;
        ring_buffer_skip(ring_buffer, 1);
    }*/

    return result;
}
/*
std::optional<std::string> extract_possible_json(Buffer& ring_buffer)
{

    if (ring_buffer.empty())
        return std::nullopt; // buffer is empty

    // Detecting possibly valid JSON string
    // Detecting '{'

    size_t buffer_data_size = ring_buffer.size();
    bool success = false;
    size_t i = 0;
    for (; i != buffer_data_size; i++)
    {
        if (ring_buffer[i] == '{')
        {
            success = true;
            break;
        }
    }
    ring_buffer.skip(i);

    if (!success)
    {
        // We reached p_write, but there are no '{', only trash
        // Wait for next try
        return std::nullopt;
    }

    uint32_t possible_json_begin = 0;

    buffer_data_size = ring_buffer.size();

    // OK, possible JSON beginning, lets find possible end
    int braces_count = 0;
    success = false;
    for(i = 0; i != buffer_data_size; i++)
    {
        char ch = ring_buffer[i];

        switch(ch)
        {
        case '{': braces_count += 1; break;
        case '}': braces_count -= 1; break;
        }

        if (braces_count == 0)
        {
            success = true;
            break;
        } else if (braces_count < 0)
        {
            success = false;
            // Next read attempt after the '}' brace
            ring_buffer.skip(i);
            // skip next???
            break;
        }
    }

    if (!success)
        return std::nullopt;

    uint32_t possible_json_end = i+1;

    size_t possible_json_size = i+1;

    std::string result(possible_json_size, 'X');

    ring_buffer.get((uint8_t*) &result[0], possible_json_size);

    return result;

}
*/
