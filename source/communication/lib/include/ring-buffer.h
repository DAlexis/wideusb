#ifndef RING_BUFFER_H_INCLUDED
#define RING_BUFFER_H_INCLUDED

#include <stdint.h>

typedef struct
{
    uint8_t *ring_buffer;
    uint32_t size;
    uint32_t p_write, p_read;
} RingBuffer;

#ifdef __cplusplus
    extern "C" {
#endif

void ring_buffer_init(RingBuffer* ring_buffer, uint8_t* buffer, uint32_t size);
uint32_t ring_buffer_free_space(const RingBuffer* ring_buffer);
uint32_t ring_buffer_data_size(const RingBuffer* ring_buffer);
void ring_buffer_put_data(RingBuffer* input_buffer, uint8_t* Buf, uint32_t Len);
void ring_buffer_get_data(RingBuffer* input_buffer, uint8_t* Buf, uint32_t Len);

#ifdef __cplusplus
    }
#endif

#endif // RING_BUFFER_H_INCLUDED
