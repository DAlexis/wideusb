#ifndef RING_BUFFER_H_INCLUDED
#define RING_BUFFER_H_INCLUDED

#include <stdint.h>

#define USBD_CDC_INPUT_RING_BUFFER_SIZE 2048

typedef struct _USBD_CDC_InputBuffer
{
    uint32_t last_time;
    uint8_t ring_buffer[USBD_CDC_INPUT_RING_BUFFER_SIZE];
    uint32_t p_write, p_read;

    uint8_t input_on_timeout;

    uint8_t has_new_data; // Will be set to 1 when new data added
} USBD_CDC_RingBuffer;

#endif // RING_BUFFER_H_INCLUDED
