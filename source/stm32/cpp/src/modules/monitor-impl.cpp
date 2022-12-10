#include "modules/monitor-impl.hpp"
#include "wideusb/communication/modules/monitor.hpp"
#include "wideusb/communication/modules/ports.hpp"
#include "os/safe-malloc-free.hpp"
#include "os/cpp-freertos.hpp"
#include "gpio.h"
#include "usart.h"

#include "newlib-monitor.h"

#include <string>
#include <mutex>

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

extern std::shared_ptr<os::Mutex> printf_mutex;

extern "C" void write_impl(char *ptr, int len)
{
    if (printf_mutex)
        printf_mutex->lock();
//    MonitorImpl::stdout_buffer.put(ptr, len);
//    while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
    HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, HAL_MAX_DELAY);
    if (printf_mutex)
        printf_mutex->unlock();
}
