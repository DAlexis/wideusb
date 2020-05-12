#ifndef RAPIDJSON_CONFIG_H_INCLUDED
#define RAPIDJSON_CONFIG_H_INCLUDED

#define RAPIDJSON_DEFAULT_STACK_ALLOCATOR   CrtAllocatorThreadSafe
#define RAPIDJSON_DEFAULT_ALLOCATOR         MemoryPoolAllocator<CrtAllocatorThreadSafe>
#define RAPIDJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY  1024

#include <stdlib.h>

class CrtAllocatorThreadSafe {
public:
    static const bool kNeedFree = true;
    void* Malloc(size_t size) {
        if (size) //  behavior of malloc(0) is implementation defined.
            return new char[size];
        else
            return NULL; // standardize to returning NULL.
    }
    void* Realloc(void* originalPtr, size_t originalSize, size_t newSize) {
        (void)originalSize;
        delete[] reinterpret_cast<char*>(originalPtr);
        if (newSize == 0) {
            return NULL;
        }
        return new char[newSize];
    }
    static void Free(void *ptr) { delete[] reinterpret_cast<char*>(ptr); }
};

#endif // RAPIDJSON_CONFIG_H_INCLUDED
