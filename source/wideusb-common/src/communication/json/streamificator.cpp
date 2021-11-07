#include "communication/json/streamificator.hpp"
#include "communication/utils.hpp"

#include "communication/json/rapidjson-config.h"
#include "rapidjson/document.h"

#include "communication/json/helpers-json.hpp"


using namespace rapidjson;

uint32_t checksum(const PBuffer data)
{
    return hash_Ly(data->data(), data->size());
}

std::optional<StreamChunkHeaderJSON> StreamChunkHeaderJSON::parse(const std::string& str)
{
    Document doc;
    doc.Parse(str.c_str());
    if (!doc.IsObject())
        return std::nullopt;

    if (!doc.HasMember("size") || !doc["size"].IsUint())
        return std::nullopt;

    StreamChunkHeaderJSON header;
    if (doc.HasMember("checksum"))
    {
        if (!doc["checksum"].IsUint())
            return std::nullopt;
        header.need_checksum = true;
        header.checksum = doc["checksum"].GetUint();
    } else {
        header.need_checksum = false;
    }
    header.size = doc["size"].GetUint();
    return header;
}

PBuffer StreamChunkHeaderJSON::serialize()
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();
/*
    d.AddMember("size", Value(size), alloc);
    if (need_checksum)
        d.AddMember("checksum", Value(checksum), alloc);*/

    return buffer_from_document(d);
}

StreamificatorJSON::StreamificatorJSON(bool need_checksum) :
    m_need_checksum(need_checksum)
{
}

bool StreamificatorJSON::pack(SerialWriteAccessor& accessor, const PBuffer buffer)
{
    StreamChunkHeaderJSON header;
    header.size = buffer->size();
    if (m_need_checksum) {
        uint32_t sum = checksum(buffer);
        header.checksum = sum;
        header.need_checksum = true;
    } else {
        header.need_checksum = false;
    }
    PBuffer header_buf = header.serialize();

    if (!accessor.will_fit(header_buf->size() + buffer->size()))
        return false;

    accessor << BufferAccessor(header_buf);
    accessor << BufferAccessor(buffer);
    return true;
}

DestreamificatorJSON::DestreamificatorJSON(bool need_checksum) :
    m_need_checksum(need_checksum)
{
}

std::optional<PBuffer> DestreamificatorJSON::unpack(SerialReadAccessor& ring_buffer)
{
    switch (m_state)
    {
    case State::waiting_header:
        {
            std::optional<std::string> result = extract_possible_json(ring_buffer);
            if (!result.has_value())
                return std::nullopt;

            std::optional<StreamChunkHeaderJSON> header = StreamChunkHeaderJSON::parse(*result);
            if (!header.has_value())
                return std::nullopt;
            if (m_need_checksum && !header->need_checksum)
            {
                // Reject requests without a checksum
                return std::nullopt;
            }
            m_header = *header;
            m_state = State::waiting_buffer;
            m_buffer_bytes_left = m_header.size;
            m_data = Buffer::create();
        } // No break here, it is ok
    case State::waiting_buffer:
        {
            uint32_t size_to_read = std::min(ring_buffer.size(), m_buffer_bytes_left);
            m_data->put(ring_buffer, size_to_read);
            m_buffer_bytes_left -= size_to_read;
            if (m_buffer_bytes_left == 0)
            {
                if (!m_need_checksum || checksum(m_data) == m_header.checksum)
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
    m_header = StreamChunkHeaderJSON();
}
