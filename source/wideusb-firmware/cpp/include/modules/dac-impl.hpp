#ifndef DAC_IMPL_HPP
#define DAC_IMPL_HPP

#include "wideusb-common/back/dac-back.hpp"

#include "module-base.hpp"

class DACImpl : public IModule, public DACModuleBackend
{
public:
    constexpr static uint16_t max_buffer_size = 1000;
    constexpr static uint16_t max_dma_chunk_size = 100;

    DACImpl(NetSevice& net_service, Address module_address);
    ~DACImpl();

    void tick() override;

    void play_next_block();

private:
    enum class RunMode
    {
        single,
        repeat,
        continious
    };

    uint16_t init_continious(uint16_t buffer_size, uint32_t prescaler, uint32_t period, uint16_t dma_chunk_size, uint16_t notify_when_left) override;
    uint16_t init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat) override;
    void run() override;
    void stop() override;
    void receive_data(PBuffer data) override;

    void notify_data_sender();

    RunMode m_run_mode = RunMode::repeat;

    uint16_t m_dma_chunk_size = max_dma_chunk_size;


    std::unique_ptr<RingBuffer> m_ring_buffer;
    PBuffer m_linear_buffer;

    uint16_t m_size_limit_notify = max_dma_chunk_size;
    bool m_stop_now = false;
    bool m_running = false;
    bool m_already_notified = false;

    MemBlock m_currently_being_played;

};

#endif // DAC_IMPL_HPP
