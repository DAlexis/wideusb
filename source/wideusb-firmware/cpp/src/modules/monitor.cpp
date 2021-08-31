#include "modules/monitor-impl.hpp"
#include "communication/modules/monitor.hpp"
#include "communication/modules/ports.hpp"

#include "newlib-monitor.h"

#include <string>

MonitorImpl::MonitorImpl(NetSevice& net_service, Address monitor_address) :
    MonitorBack(net_service, monitor_address)
{
    m_sock.options().need_acknoledgement = false;
    m_sock.options().retransmitting_options.cycles_count = 1;
    m_sock.options().retransmitting_options.timeout = 0;
}

void MonitorImpl::tick()
{

}

uint32_t MonitorImpl::get_heap_used()
{
    return heap_used;
}

uint32_t MonitorImpl::get_heap_total()
{
    return heap_total;
}
