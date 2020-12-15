#ifndef JSONHELPERS_HPP
#define JSONHELPERS_HPP

#include "buffer.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"

PBuffer buffer_from_document(const rapidjson::Document& doc);

#endif // JSONHELPERS_HPP
