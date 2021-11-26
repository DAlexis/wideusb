#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "wideusb/communication/networking.hpp"
#include "wideusb/back/monitor-back.hpp"

class MonitorImpl : public MonitorBack
{
public:
    MonitorImpl(NetService& net_service, Address monitor_address);

    void tick() override;

    uint32_t get_heap_used() override;
    uint32_t get_heap_total() override;

private:

};

#endif // MONITOR_HPP
