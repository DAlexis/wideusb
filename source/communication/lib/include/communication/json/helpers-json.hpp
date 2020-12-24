#ifndef JSONHELPERS_HPP
#define JSONHELPERS_HPP

#include "buffer.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"

#include <string>
#include <optional>

PBuffer buffer_from_document(const rapidjson::Document& doc);
std::optional<std::string> extract_possible_json(SerialReadAccessor& ring_buffer);

#endif // JSONHELPERS_HPP
