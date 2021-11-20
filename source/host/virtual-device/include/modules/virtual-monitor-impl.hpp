#ifndef VIRTUALMONITORIMPL_HPP
#define VIRTUALMONITORIMPL_HPP

#include "wideusb-common/back/monitor-back.hpp"
#include "wideusb-common/module-base.hpp"

class MonitorBackImpl : public MonitorBack, public IModule
{
public:
    using MonitorBack::MonitorBack;

    void tick() override;

    uint32_t get_heap_used() override;
    uint32_t get_heap_total() override;
};


#endif // VIRTUALMONITORIMPL_HPP
