#ifndef DAC_HPP
#define DAC_HPP

#include "module-base.hpp"
#include "communication/networking.hpp"

class DACModule : public IModule
{
public:
    constexpr static uint16_t max_buffer_size = 1000;
    constexpr static uint16_t max_dma_chunk_size = 100;
    DACModule(NetSevice& net_service, Address module_address);
    ~DACModule();

    void tick() override;

    void play_next_block();
private:
    enum class RunMode
    {
        single,
        repeat,
        continious
    };

    void sock_setup_listener();
    void sock_data_listener();
    void run();
    void stop();
    void notify_data_sender();


    /**
     * @brief init
     * @param buffer_size
     * @param m_prescaler
     * @param m_period
     * @param mode
     * @return Actual size of crated buffer
     */
    uint16_t init(uint16_t buffer_size, uint32_t m_prescaler, uint32_t m_period, RunMode mode);

    std::unique_ptr<RingBuffer> m_ring_buffer;
    PBuffer m_linear_buffer;

    PBuffer m_temp_buffer;

    RunMode m_run_mode = RunMode::single;

    Socket m_sock_setup;
    Socket m_sock_data;

    Address m_status_reports_receiver = 0;
    uint16_t m_size_limit_notify = max_dma_chunk_size;
    bool m_stop_now = false;
    bool m_already_notified = false;

    MemBlock m_currently_being_played;

};

#endif // DAC_HPP
