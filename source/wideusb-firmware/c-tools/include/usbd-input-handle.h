#ifndef USBDINPUTHANDLE_H
#define USBDINPUTHANDLE_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

void usbd_rx_handler(const uint8_t* buffer, uint32_t size);

#ifdef __cplusplus
    }
#endif


//#include "ring-buffer.h"
/*
typedef struct
{
    RingBuffer buffer;
    uint32_t last_receive_time;
    uint8_t buffer_on_timeout;
} USBDInputHandle;

#ifdef __cplusplus
    extern "C" {
#endif

void usbd_input_handle_init(USBDInputHandle* handle, uint8_t* buffer, uint32_t size);
void usbd_input_handle_receive(USBDInputHandle* handle, uint8_t* Buf, uint32_t Len, uint32_t time, uint32_t input_timeout);

#ifdef __cplusplus
    }
#endif*/

#endif // USBDINPUTHANDLE_H
