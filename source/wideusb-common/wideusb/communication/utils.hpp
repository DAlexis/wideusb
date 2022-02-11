#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <chrono>

uint32_t hash_Ly(uint8_t next_byte, uint32_t prev_hash);
uint32_t hash_Ly(const void * buf, uint32_t size, uint32_t hash = 0);

#endif // UTILS_HPP
