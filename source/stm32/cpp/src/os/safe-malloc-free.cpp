#include "os/safe-malloc-free.hpp"
#include "os/cpp-freertos.hpp"
#include "FreeRTOS.h"
#include "task.h"

#include <cstdlib>

HeapStats heap_stats;

HeapStats get_heap_stats()
{
    return heap_stats;
}

extern "C" {

void* __real_malloc(size_t);
void __real_free (void *);

void* __wrap_malloc(size_t size)
{
    void *pvReturn;
    if (os::is_inside_interrupt())
    {
        os::CriticalSectionISR cs;
        pvReturn = __real_malloc( size );
        cs.unlock();
        heap_stats.malloc_isr_times++;
    } else {
        os::CriticalSection cs;
        pvReturn = __real_malloc( size );
        cs.unlock();
    }
    heap_stats.allocated += size;
    heap_stats.malloc_times++;
    return pvReturn;
}

void __wrap_free(void *pv)
{
    if (os::is_inside_interrupt())
    {
        os::CriticalSectionISR cs;
        __real_free( pv );
        cs.unlock();
        heap_stats.free_isr_times++;
    } else {
        os::CriticalSection cs;
        __real_free( pv );
        cs.unlock();
    }

    heap_stats.free_times++;
}

}
