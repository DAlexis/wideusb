#include "communication/json/streamificator.hpp"
#include "communication/utils.hpp"

#include "json/ring-buffer-json.hpp"
#include "rapidjson-config.h"
#include "rapidjson/document.h"

#include "json/helpers-json.hpp"


using namespace rapidjson;

uint32_t checksum(const PBuffer data)
{
    return hash_Ly(data->data(), data->size());
}

std::optional<Header> Header::parse(const std::string& str)
{
    Document doc;
    doc.Parse(str.c_str());
    if (!doc.IsObject())
        return std::nullopt;

    if (!doc.HasMember("checksum") || !doc["checksum"].IsUint())
        return std::nullopt;

    if (!doc.HasMember("size") || !doc["size"].IsUint())
        return std::nullopt;

    Header header;
    header.checksum = doc["checksum"].GetUint();
    header.size = doc["size"].GetUint();
    return header;
}

PBuffer Header::serialize()
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    d.AddMember("size", Value(size), alloc);
    d.AddMember("checksum", Value(checksum), alloc);

    return buffer_from_document(d);
}

std::optional<PBuffer> DestreamificatorJSON::unpack(RingBuffer& ring_buffer)
{
    switch (m_state)
    {
    case State::waiting_header:
        {
            std::optional<std::string> result = extract_possible_json(&ring_buffer);
            if (!result.has_value())
                return std::nullopt;

            std::optional<Header> header = Header::parse(*result);
            if (!header.has_value())
                return std::nullopt;
            m_header = *header;
            m_state = State::waiting_buffer;
            m_buffer_bytes_left = m_header.size;
            m_data = Buffer::create();
        } // No break here, it is ok
    case State::waiting_buffer:
        {
            uint32_t size_to_read = std::min(ring_buffer_data_size(&ring_buffer), m_buffer_bytes_left);
            m_data->append(ring_buffer, size_to_read);
            m_buffer_bytes_left -= size_to_read;
            if (m_buffer_bytes_left == 0)
            {
                if (checksum(m_data) == m_header.checksum)
                {
                    reset();
                    return m_data;
                }
                reset();
            }
        }
    }
    return std::nullopt;
}

void DestreamificatorJSON::reset()
{
    m_state = State::waiting_header;
    m_buffer_bytes_left = 0;
    m_header = Header();
}

bool StreamificatorJSON::pack(RingBuffer& ring_buffer, const PBuffer buffer)
{
    uint32_t sum = checksum(buffer);
    Header header;
    header.size = buffer->size();
    header.checksum = sum;
    PBuffer header_buf = header.serialize();

    if (header_buf->size() + buffer->size() > ring_buffer_free_space(&ring_buffer))
        return false;

    *header_buf >> ring_buffer;
    *buffer >> ring_buffer;
    return true;
}
