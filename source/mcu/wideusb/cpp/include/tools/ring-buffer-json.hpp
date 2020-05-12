#ifndef RING_BUFFER_JSON_HPP_INCLUDED
#define RING_BUFFER_JSON_HPP_INCLUDED

#include "tools/ring-buffer.h"
#include <string>
#include <optional>

std::optional<std::string> extract_json(USBD_CDC_RingBuffer* ring_buffer);

#endif // RING_BUFFER_JSON_HPP_INCLUDED
