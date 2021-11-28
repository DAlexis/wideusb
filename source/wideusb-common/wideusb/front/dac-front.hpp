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

    using OnInitDoneCallback = std::function<void(int)>;
    using OnDataSampleSet = std::function<void(bool)>;
    using OnRun = std::function<void(bool)>;
    using OnStop = std::function<void(bool)>;
    using OnBufferIsShort = std::function<void(size_t)>;

    DACFront(NetService& host_connection_service, OnModuleCreatedCallback on_created, Address my_address = 0, Address device_address = 0);

    void init_continious(uint16_t dma_chunk_size, uint32_t prescaler, uint32_t period, OnInitDoneCallback on_init_done, OnBufferIsShort on_buffer_short);
    void init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat, OnInitDoneCallback on_init_done);

    void send_data(const float* data, size_t size, OnDataSampleSet on_data_sample_set);

    void run(OnRun on_run);
    void stop(OnStop on_stop);

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

    OnDataSampleSet m_on_data_sample_set;
    SegmentID m_data_set_segment_id;


    bool m_dac_initialized = false;
    size_t m_buffer_size = 0;
    OnInitDoneCallback m_on_init_done;
    OnRun m_on_run;
    OnStop m_on_stop;
    OnBufferIsShort m_on_buffer_short;
    SegmentID m_run_segment_id;
    SegmentID m_stop_segment_id;

};

#endif // DAC_FRONT_HPP
