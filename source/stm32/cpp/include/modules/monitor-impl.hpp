#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "wideusb/communication/networking.hpp"
#include "wideusb/back/monitor-back.hpp"

class MonitorImpl : public MonitorBack
{
public:
    MonitorImpl(NetService::ptr net_service, Address monitor_address);

    void tick() override;

    MonitorStats get_stats() override;

    static RingBuffer stdout_buffer;

    constexpr static size_t stdout_chunk_size = 100;

private:

};

#endif // MONITOR_HPP
