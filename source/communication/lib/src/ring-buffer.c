#include "ring-buffer.h"
#include <string.h>

void ring_buffer_init(RingBuffer* ring_buffer, uint8_t* buffer, uint32_t size)
{
    memset(ring_buffer, 0, sizeof(RingBuffer));
    ring_buffer->ring_buffer = buffer;
    ring_buffer->size = size;
}

uint32_t ring_buffer_free_space(const RingBuffer* ring_buffer)
{
    if (ring_buffer->p_read <= ring_buffer->p_write)
    {
        return ring_buffer->size + ring_buffer->p_read - ring_buffer->p_write - 1;
    } else {
        return ring_buffer->p_read - ring_buffer->p_write - 1;
    }
}

uint32_t ring_buffer_data_size(const RingBuffer* ring_buffer)
{
    if (ring_buffer->p_read <= ring_buffer->p_write)
    {
        return ring_buffer->p_write - ring_buffer->p_read;
    } else {
        return ring_buffer->size + ring_buffer->p_write - ring_buffer->p_read;
    }
}

void ring_buffer_put_data(RingBuffer* buffer, const uint8_t* buf, uint32_t len)
{
    uint32_t free_tail = buffer->size - buffer->p_write;
    if (len < free_tail)
    {
        // Add to the end
        memcpy(&buffer->ring_buffer[buffer->p_write], buf, len);
        buffer->p_write += len;
    } else {
        // Part add to the end and part add to the beginning
        memcpy(&buffer->ring_buffer[buffer->p_write], buf, free_tail);
        uint32_t second_part_size = len - free_tail;
        memcpy(&buffer->ring_buffer[0], &buf[free_tail], second_part_size);
        buffer->p_write = second_part_size;
    }
}

void ring_buffer_get_data(RingBuffer* buffer, uint8_t* buf, uint32_t len)
{
    if (buffer->p_write >= buffer->p_read)
    {
        memcpy(buf, &buffer->ring_buffer[buffer->p_read], len);
        buffer->p_read += len;
    } else {
        uint32_t tail = buffer->size - buffer->p_read;
        if (tail > len)
        {
            memcpy(buf, &buffer->ring_buffer[buffer->p_read], len);
            buffer->p_read += len;
        } else {
            memcpy(buf, &buffer->ring_buffer[buffer->p_read], tail);
            memcpy(buf+tail, &buffer->ring_buffer[0], len - tail);
            buffer->p_read = len - tail;
        }
    }
}

void ring_buffer_skip(RingBuffer* ring_buffer, uint32_t len)
{
    ring_buffer->p_read += len;
    if (ring_buffer->p_read >= ring_buffer->size)
        ring_buffer->p_read -= ring_buffer->size;
}

uint8_t* ring_buffer_at(RingBuffer* ring_buffer, uint32_t pos)
{
    uint32_t target_pos = pos + ring_buffer->p_read;
    if (target_pos > ring_buffer->size)
    {
        target_pos -= ring_buffer->size;
    }
    return &ring_buffer->ring_buffer[target_pos];
}
