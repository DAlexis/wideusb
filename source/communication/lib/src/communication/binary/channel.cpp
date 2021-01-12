#ifndef CHANNEL_CPP
#define CHANNEL_CPP

#include "communication/binary/channel.hpp"
#include "communication/utils.hpp"

ChannelLayerBinary::ChannelLayerBinary()
{
}

PBuffer ChannelLayerBinary::find_sucessful_instance(RingBuffer& ring_buffer)
{
    for (auto it = m_decoding_instances.begin(); it != m_decoding_instances.end();)
    {
        size_t size = it->header.size;
        if (size  + it->body_begin > ring_buffer.size())
        {
            ++it;
            continue;
        }

        // Try to test
        uint32_t hash_value = 0;
        for (size_t i = 0; i != size; i++)
        {
            hash_value = hash_Ly(&ring_buffer[i + it->body_begin], 1, hash_value);
        }

        if (hash_value != it->header.checksum)
        {
            // Block is incorrect, bad checksum
            it = m_decoding_instances.erase(it);
            continue;
        }

        size_t this_block_begin = it->body_begin;
        size_t this_block_end = this_block_begin + size;

        // Seccess, we have correct block
        PBuffer pbuf = Buffer::create(size);
        ring_buffer.skip(this_block_begin);
        ring_buffer.extract(pbuf->data(), size);

        // Erasing all blocks that intersect current block

        // First, removing all previously found parsing instances, that are already in list.
        // They must intersect our block, otherwise they are already checked and discarded by hash
        for (auto jt = m_decoding_instances.begin(); jt != it; )
        {
            jt = m_decoding_instances.erase(jt);
        }

        m_decoding_instances.erase(it);

        for (auto jt = m_decoding_instances.begin(); jt != m_decoding_instances.end(); )
        {
            if (jt->body_begin < this_block_end)
            {
                // We have intersection
                jt = m_decoding_instances.erase(jt);
            } else {
                jt->body_begin -= this_block_end;
                ++jt;
            }
        }

        if (m_header_search_pos > this_block_end)
            m_header_search_pos -= this_block_end;
        else
            m_header_search_pos = 0;
        return pbuf;

    }
    return nullptr;
}

void ChannelLayerBinary::find_next_headers(RingBuffer& ring_buffer)
{
    if (ring_buffer.size() < sizeof(ChannelHeader))
        return;

    if (m_decoding_instances.empty())
    {
        // we had not detected header before, so rewind buffer to headers begin
        // TODO add code here
        ring_buffer.skip(m_header_search_pos);
        size_t search_limit = ring_buffer.size() - sizeof(ChannelHeader);
        for (m_header_search_pos = 0; m_header_search_pos < search_limit; ++m_header_search_pos)
        {
            if (ring_buffer.as<uint32_t>(m_header_search_pos) == ChannelHeader::magic_number_value)
                break;
        }
        ring_buffer.skip(m_header_search_pos);
        m_header_search_pos = 0;
    }

    size_t search_limit = ring_buffer.size() - sizeof(ChannelHeader);
    for (size_t i = m_header_search_pos; i < search_limit; ++i)
    {
        if (ring_buffer.as<uint32_t>(i) != ChannelHeader::magic_number_value)
            continue;

        DecodingInstance new_instance;
        new_instance.body_begin = i + sizeof(ChannelHeader);
        new_instance.header = ring_buffer.as<ChannelHeader>(i);

        m_decoding_instances.push_back(new_instance);
    }
    m_header_search_pos = search_limit;
}

PBuffer ChannelLayerBinary::decode_single(RingBuffer& ring_buffer)
{
    find_next_headers(ring_buffer);
    return find_sucessful_instance(ring_buffer);
}

std::vector<DecodedFrame> ChannelLayerBinary::decode(RingBuffer& ring_buffer)
{
    std::vector<DecodedFrame> result;

    PBuffer next_frame_contents = nullptr;
    for (;;)
    {
        next_frame_contents = decode_single(ring_buffer);
        if (!next_frame_contents)
            break;
        result.push_back(DecodedFrame(BufferAccessor(next_frame_contents)));
    }

    return result;
}

void ChannelLayerBinary::encode(SegmentBuffer& frame)
{
    // TODO make it without merge call
    PBuffer merged = frame.merge();
    ChannelHeader header;
    header.checksum = hash_Ly(merged->data(), merged->size());
    header.size = merged->size();
    frame.push_front(Buffer::create(sizeof(header), &header));
}

#endif // CHANNEL_CPP
