#ifndef RING_BUFFER_JSON_HPP_INCLUDED
#define RING_BUFFER_JSON_HPP_INCLUDED

#include "ring-buffer.h"
#include <string>
#include <optional>

std::optional<std::string> extract_possible_json(RingBuffer* ring_buffer);

#endif // RING_BUFFER_JSON_HPP_INCLUDED
