#include "modules/monitor.hpp"
#include "communication/modules/monitor.hpp"

#include "newlib-monitor.h"

#include <string>

MonitorModule::MonitorModule(NetSevice& net_service, Address monitor_address, Address destination_address, Port port) :
    m_sock(net_service, monitor_address, destination_address, port)
{
    m_sock.options().need_acknoledgement = false;
    m_sock.options().retransmitting_options.cycles_count = 1;
    m_sock.options().retransmitting_options.timeout = 0;
}

void MonitorModule::make_report()
{
    std::string message = "WideUSB device status:\n";
    message = message + "Heap: " + std::to_string(heap_used) + " of " + std::to_string(heap_total) + "\n";

    PBuffer b = Buffer::create(message.size()+1, message.data());
    m_sock.send(b);
}