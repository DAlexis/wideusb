#include "modules/monitor-impl.hpp"
#include "wideusb/communication/modules/monitor.hpp"
#include "wideusb/communication/modules/ports.hpp"
#include "os/safe-malloc-free.hpp"
#include "gpio.h"

#include "newlib-monitor.h"

#include <string>

RingBuffer MonitorImpl::stdout_buffer{1000};

MonitorImpl::MonitorImpl(NetService::ptr net_service, Address monitor_address) :
    MonitorBack(net_service, monitor_address)
{

}

void MonitorImpl::tick()
{
    if (!has_stdout_subscriber())
        return;

    while(stdout_buffer.size() > 0)
    {
        size_t chunk_size = std::min(stdout_chunk_size, stdout_buffer.size());
        auto buf = stdout_buffer.extract_buf(chunk_size);
        send_stdout(buf);
    }
}

MonitorStats MonitorImpl::get_stats()
{
    HeapStats heap_stats = get_heap_stats();
    MonitorStats result;
    result.heap_total = heap_total;
    result.heap_used = heap_used;
    result.allocated = heap_stats.allocated;
    result.malloc_times = heap_stats.malloc_times;
    result.malloc_isr_times = heap_stats.malloc_isr_times;
    result.free_times = heap_stats.free_times;
    result.free_isr_times = heap_stats.free_isr_times;
    return result;
}

extern "C" void write_impl(char *ptr, int len)
{
    MonitorImpl::stdout_buffer.put(ptr, len);

}
