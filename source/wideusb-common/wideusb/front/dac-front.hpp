#ifndef DAC_FRONT_HPP
#define DAC_FRONT_HPP

#include "wideusb/front/base-front.hpp"

class DACFront : public ModuleFrontBase
{
public:
    enum class Mode
    {
        single,
        repeat,
        continious
    };

    using OnInitDoneCallbackEntryPoint = CallbackEntry<int>;
    using OnDataSampleSetEntryPoint = CallbackEntry<bool>;
    using OnRunEntryPoint = CallbackEntry<bool>;
    using OnStopEntryPoint = CallbackEntry<bool>;
    using OnBufferIsShortEntryPoint = CallbackEntry<size_t>;

    DACFront(NetService& host_connection_service, OnModuleCreatedCallbackEntry on_created, Address my_address = 0, Address device_address = 0);

    void init_continious(uint16_t dma_chunk_size, uint32_t prescaler, uint32_t period, OnInitDoneCallbackEntryPoint on_init_done, OnBufferIsShortEntryPoint on_buffer_short);
    void init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat, OnInitDoneCallbackEntryPoint on_init_done);

    void send_data(const float* data, size_t size, OnDataSampleSetEntryPoint on_data_sample_set);

    void run(OnRunEntryPoint on_run);
    void stop(OnStopEntryPoint on_stop);

private:
    void sock_setup_listener();
    void sock_data_listener();
    void sock_setup_on_received(uint32_t id, bool success);
    void sock_data_on_received(uint32_t id, bool success);

    static void prepare_to_send(uint16_t* dst, const float* data, size_t size);

    PBuffer m_data;

    Socket m_sock_setup;
    Socket m_sock_data;

    std::list<PBuffer> m_chunks;

    OnDataSampleSetEntryPoint m_on_data_sample_set;
    SegmentID m_data_set_segment_id;


    bool m_dac_initialized = false;
    size_t m_buffer_size = 0;
    OnInitDoneCallbackEntryPoint m_on_init_done;
    OnRunEntryPoint m_on_run;
    OnStopEntryPoint m_on_stop;
    OnBufferIsShortEntryPoint m_on_buffer_short;
    SegmentID m_run_segment_id;
    SegmentID m_stop_segment_id;

};

#endif // DAC_FRONT_HPP
