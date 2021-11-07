#include "modules/monitor-impl.hpp"
#include "wideusb-common/communication/modules/monitor.hpp"
#include "wideusb-common/communication/modules/ports.hpp"

#include "gpio.h"

#include "newlib-monitor.h"

#include <string>

MonitorImpl::MonitorImpl(NetService& net_service, Address monitor_address) :
    MonitorBack(net_service, monitor_address)
{

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
