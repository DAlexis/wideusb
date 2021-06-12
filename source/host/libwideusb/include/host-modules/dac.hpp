#ifndef HOST_MODULE_DAC_GPP
#define HOST_MODULE_DAC_GPP

#include "communication/networking.hpp"
#include "host-module.hpp"


namespace WideUSBHost
{

class Device;

namespace modules
{

class DAC : public ModuleBase
{
public:
    enum class Mode
    {
        single,
        repeat,
        continious
    };

    using OnInitDoneCallback = std::function<void(int)>;
    using OnDataSampleSet = std::function<void(size_t)>;
    using OnRun = std::function<void(bool)>;

    DAC(Device& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address = 0, Address custom_device_address = 0);

    void init_continious(uint16_t buffer_size, uint32_t prescaler, uint32_t period, uint16_t dma_chunk_size, uint16_t notify_when_left, OnInitDoneCallback on_init_done);
    void init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat, OnInitDoneCallback on_init_done);

    void send_data(const std::vector<float>& data, OnDataSampleSet on_data_sample_set);
    void add_continious_data_chunk(const std::vector<float>& data);

    void run(OnRun callback);
    void stop();

private:
    void sock_setup_listener();
    void sock_data_lestener();
    void sock_setup_on_received_by_device(uint32_t id, bool success);
    void sock_data_on_received(uint32_t id, bool success);


    static std::vector<uint16_t> prepare_to_send(const std::vector<float>& data);

    PBuffer m_data;

    Socket m_sock_setup;
    Socket m_sock_data;

    std::list<PBuffer> m_chunks;

    PBuffer m_sample;
    OnDataSampleSet m_on_data_sample_set;
    SegmentID m_data_set_segment_id;


    bool m_dac_initialized = false;
    size_t m_buffer_size = 0;
    OnInitDoneCallback m_on_init_done;
    OnRun m_on_run_callback;
    SegmentID m_run_segment_id;

};

}
}

#endif // HOST_MODULE_DAC_GPP
