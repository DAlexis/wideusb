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

#endif // USBDINPUTHANDLE_H
