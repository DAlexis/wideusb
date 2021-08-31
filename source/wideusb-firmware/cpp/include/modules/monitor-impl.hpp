#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "communication/networking.hpp"
#include "module-base.hpp"
#include "wideusb-common/back/monitor-back.hpp"

class MonitorImpl : public MonitorBack, public IModule
{
public:
    MonitorImpl(NetSevice& net_service, Address monitor_address);

    void tick();

    uint32_t get_heap_used() override;
    uint32_t get_heap_total() override;

private:

};

#endif // MONITOR_HPP
