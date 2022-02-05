#ifndef SAFEMALLOCFREE_HPP
#define SAFEMALLOCFREE_HPP

#include <cstdlib>

struct HeapStats
{
    size_t allocated = 0;
    size_t malloc_times = 0;
    size_t malloc_isr_times = 0;
    size_t free_times = 0;
    size_t free_isr_times = 0;
};

HeapStats get_heap_stats();

#endif // SAFEMALLOCFREE_HPP
