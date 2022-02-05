#ifndef MONITORTYPES_HPP
#define MONITORTYPES_HPP

#include <cstdlib>

struct MonitorStats
{
    size_t heap_total = 0;
    size_t heap_used = 0;
    size_t allocated = 0;
    size_t malloc_times = 0;
    size_t malloc_isr_times = 0;
    size_t free_times = 0;
    size_t free_isr_times = 0;
};

#endif // MONITORTYPES_HPP
