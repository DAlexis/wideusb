#include "communication/utils.hpp"

uint32_t hash_Ly(uint8_t next_byte, uint32_t prev_hash)
{
    return (prev_hash * 1664525) + next_byte + 1013904223;
}

uint32_t hash_Ly(const void * buf, uint32_t size, uint32_t hash)
{
    for(uint32_t i=0; i<size; i++)
        hash = hash_Ly(reinterpret_cast<const uint8_t*>(buf)[i], hash);

    return hash;
}
