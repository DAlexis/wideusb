#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "wideusb-common/communication/networking.hpp"
#include "wideusb-common/module-base.hpp"
#include "wideusb-common/back/monitor-back.hpp"

class MonitorImpl : public MonitorBack, public IModule
{
public:
    MonitorImpl(NetService& net_service, Address monitor_address);

    void tick();

    uint32_t get_heap_used() override;
    uint32_t get_heap_total() override;

private:

};

#endif // MONITOR_HPP
