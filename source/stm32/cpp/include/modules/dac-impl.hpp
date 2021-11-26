#ifndef DAC_IMPL_HPP
#define DAC_IMPL_HPP

#include "wideusb/back/dac-back.hpp"

class DACImpl : public DACModuleBackend
{
public:
    constexpr static uint16_t max_buffer_size = 1000;
    constexpr static uint16_t max_dma_chunk_size = 1000;

    DACImpl(NetService& net_service, Address module_address);
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
    void stop_immediately() override;
    void receive_data(PBuffer data) override;

    void notify_data_sender();
    void play_next_continious_block();

    RunMode m_run_mode = RunMode::repeat;

    uint16_t m_dma_chunk_size = max_dma_chunk_size;

    PBuffer m_linear_buffer;

    PBuffer m_continious_buffer_1;
    PBuffer m_continious_buffer_2;

    PBuffer* m_continious_buffer_current = nullptr;
    PBuffer* m_continious_buffer_next = nullptr;
    size_t m_buffer_filling_current = 0;
    size_t m_buffer_filling_next = 0;

    bool m_stop_now = false;
    bool m_running = false;
    bool m_already_notified = true;

};

#endif // DAC_IMPL_HPP
