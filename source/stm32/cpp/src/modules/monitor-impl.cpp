#include "modules/monitor-impl.hpp"
#include "wideusb/communication/modules/monitor.hpp"
#include "wideusb/communication/modules/ports.hpp"

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

uint32_t MonitorImpl::get_heap_used()
{
    return heap_used;
}

uint32_t MonitorImpl::get_heap_total()
{
    return heap_total;
}


extern "C" void write_impl(char *ptr, int len)
{
    MonitorImpl::stdout_buffer.put(ptr, len);

}
