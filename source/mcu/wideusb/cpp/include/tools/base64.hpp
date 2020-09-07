#ifndef BASE64_HPP_INCLUDED
#define BASE64_HPP_INCLUDED

#include <string>
#include <vector>
#include <optional>

#include "stddef.h"

std::optional<std::string> base64_encode(const std::vector<uint8_t> src);
std::optional<std::string> base64_encode(const uint8_t *src, size_t len);

std::optional<std::vector<uint8_t>> base64_decode(const std::string& src_string);

#endif // BASE64_HPP_INCLUDED
