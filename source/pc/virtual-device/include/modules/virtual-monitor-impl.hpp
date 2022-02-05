#ifndef VIRTUALMONITORIMPL_HPP
#define VIRTUALMONITORIMPL_HPP

#include "wideusb/back/monitor-back.hpp"
#include "wideusb/back/base-back.hpp"

class MonitorBackImpl : public MonitorBack
{
public:
    using MonitorBack::MonitorBack;

    void tick() override;

    MonitorStats get_stats() override;
};


#endif // VIRTUALMONITORIMPL_HPP
