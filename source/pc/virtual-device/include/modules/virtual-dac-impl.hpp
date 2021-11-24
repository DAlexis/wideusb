#ifndef VIRTUALDACIMPL_HPP
#define VIRTUALDACIMPL_HPP

#include "wideusb/back/dac-back.hpp"
#include "wideusb/back/base-back.hpp"

class VirtualDacImpl : public DACModuleBackend
{
public:
    using DACModuleBackend::DACModuleBackend;

private:
    void tick() override;

    uint16_t init_continious(uint16_t buffer_size, uint32_t prescaler, uint32_t period, uint16_t dma_chunk_size, uint16_t notify_when_left) override;
    uint16_t init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat) override;
    void run() override;
    void stop() override;
    void receive_data(PBuffer data) override;

    // todo Add chrono here, imitation and log according to time
};


#endif // VIRTUALDACIMPL_HPP
