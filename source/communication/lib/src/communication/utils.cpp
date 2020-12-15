#include "communication/utils.hpp"

uint32_t hash_Ly(const void * buf, uint32_t size, uint32_t hash)
{
    for(uint32_t i=0; i<size; i++)
        hash = (hash * 1664525) + reinterpret_cast<const uint8_t*>(buf)[i] + 1013904223;

    return hash;
}
