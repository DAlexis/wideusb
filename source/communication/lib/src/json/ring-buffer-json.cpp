#include "json/ring-buffer-json.hpp"

static uint32_t next(uint32_t i, uint32_t size)
{
    if (i == size-1)
        return 0;
    else
        return i+1;
}

std::optional<std::string> extract_possible_json(RingBuffer* ring_buffer)
{
    /* Buffer content may be extended during this function call,
     * when interrupt may be called.
     */

    if (ring_buffer->p_read == ring_buffer->p_write)
        return std::nullopt; // buffer is empty

    // Detecting possibly valid JSON string
    // Detecting '{'
    uint32_t i = ring_buffer->p_read;
    bool success = false;
    for (;;)
    {
        if (i == ring_buffer->p_write)
        {
            success = false;
            break;
        }

        if (ring_buffer->ring_buffer[i] == '{')
        {
            success = true;
            break;
        }

        i = next(i, ring_buffer->size);
    }
    ring_buffer->p_read = i;
    if (!success)
    {
        // We reached p_write, but there are no '{', only trash
        // Wait for next try
        return std::nullopt;
    }
    uint32_t possible_json_begin = i;

    // OK, possible JSON beginning, lets find possible end
    int braces_count = 0;
    for(;;)
    {
        if (i == ring_buffer->p_write)
        {
            success = false;
            break;
        }

        if (ring_buffer->ring_buffer[i] == '{')
            braces_count += 1;
        else if (ring_buffer->ring_buffer[i] == '}')
            braces_count -= 1;

        if (braces_count == 0)
        {
            success = true;
            break;
        } else if (braces_count < 0)
        {
            success = false;
            // Next read attempt after the '}' brace
            ring_buffer->p_read = next(i, ring_buffer->size);
            break;
        }

        i = next(i, ring_buffer->size);
    }

    if (success)
    {
        uint32_t possible_json_end = next(i, ring_buffer->size);

        std::string result;
        if (possible_json_begin < possible_json_end)
        {
            result.append(&ring_buffer->ring_buffer[possible_json_begin], &ring_buffer->ring_buffer[possible_json_end]);
        } else {
            result.append(&ring_buffer->ring_buffer[possible_json_begin], &ring_buffer->ring_buffer[ring_buffer->size]);
            result.append(&ring_buffer->ring_buffer[0], &ring_buffer->ring_buffer[possible_json_end]);
        }
        ring_buffer->p_read = possible_json_end;

        // removing CR, LF, \0 from buffer
        while (ring_buffer_data_size(ring_buffer) >= 1)
        {
            uint8_t c = *ring_buffer_at(ring_buffer, 0);
            if (c != '\r' && c != '\n' && c != '\0')
                break;
            ring_buffer_skip(ring_buffer, 1);
        }

        return result;
    }
    return std::nullopt;
}
