#include "tools/ring-buffer-json.hpp"

std::optional<std::string> extract_json(USBD_CDC_RingBuffer* ring_buffer)
{
    /* Buffer content may be extended during this function call,
     * when interrupt may be called.
     */
    if (ring_buffer->input_on_timeout)
    {
        return std::nullopt;
    }

    if (ring_buffer->p_read == ring_buffer->p_write)
        return std::nullopt; // buffer is empty

    // Detecting possibly valid JSON string
    // Detecting '{'
    uint32_t i = ring_buffer->p_read;
    bool stop = false;
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

        if (i == USBD_CDC_INPUT_RING_BUFFER_SIZE-1)
            i = 0;
        else
            i++;
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
            ring_buffer->p_read = i+1;
            break;
        }

        if (i == USBD_CDC_INPUT_RING_BUFFER_SIZE-1)
            i = 0;
        else
            i++;
    }

    if (success)
    {
        uint32_t possible_json_end = i+1;
        std::string result;
        if (possible_json_begin < possible_json_end)
        {
            result.append(&ring_buffer->ring_buffer[possible_json_begin], &ring_buffer->ring_buffer[possible_json_end]);
        } else {
            result.append(&ring_buffer->ring_buffer[possible_json_begin], &ring_buffer->ring_buffer[USBD_CDC_INPUT_RING_BUFFER_SIZE]);
            result.append(&ring_buffer->ring_buffer[0], &ring_buffer->ring_buffer[possible_json_end]);
        }
        ring_buffer->p_read = possible_json_end;
        return result;
    }
    return std::nullopt;
}
