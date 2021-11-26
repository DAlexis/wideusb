#include "usbd-input-handle.h"
/*
void usbd_input_handle_receive(USBDInputHandle* handle, uint8_t* buf, uint32_t len, uint32_t time, uint32_t input_timeout)
{
    if (handle->buffer_on_timeout)
    {
        if (time - handle->last_receive_time > input_timeout)
        {
            // Timeout is over
            handle->buffer_on_timeout = 0;
        }
    }

    if (!handle->buffer_on_timeout)
    {
        if (len < ring_buffer_free_space(&handle->buffer))
        {
            ring_buffer_put_data(&handle->buffer, buf, len);
        } else {
            handle->buffer_on_timeout = 1;
        }
    }
    handle->last_receive_time = time;
}

void usbd_input_handle_init(USBDInputHandle* handler, uint8_t* buffer, uint32_t size)
{
    ring_buffer_init(&handler->buffer, buffer, size);
    handler->last_receive_time = 0;
    handler->buffer_on_timeout = 0;
}
*/
