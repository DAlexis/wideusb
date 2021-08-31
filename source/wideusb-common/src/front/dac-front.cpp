#include "wideusb-common/front/dac-front.hpp"

#include "communication/modules/dac.hpp"
//#include "wideusb.hpp"

//#include <iostream>

#include <cmath>

DACFront::DACFront(NetSevice& host_connection_service, OnModuleCreatedCallback on_created, Address my_address, Address device_address) :
    ModuleFrontBase(
        host_connection_service, dac::id,
        device_address,
        my_address,
        on_created
    ),
    m_sock_setup(
        m_host_connection_service,
        my_address,
        dac::setup::port,
        [this](ISocketUserSide&) { sock_setup_listener(); },
        [this](uint32_t id, bool success) { sock_setup_on_received_by_device(id, success); }
    ),
    m_sock_data(
        m_host_connection_service,
        my_address,
        dac::data::port,
        [this](ISocketUserSide&) { sock_data_lestener(); },
        [this](uint32_t id, bool success) { sock_data_on_received(id, success); }
    )
{
}

void DACFront::init_continious(uint16_t buffer_size, uint32_t prescaler, uint32_t period, uint16_t dma_chunk_size, uint16_t notify_when_left, OnInitDoneCallback on_init_done)
{
    dac::setup::InitContinious request;
    request.timings.period = period;
    request.timings.prescaler = prescaler;
    request.buffer_size = buffer_size;
    request.chunk_size = dma_chunk_size;
    request.notify_when_left = notify_when_left;

    m_on_init_done = on_init_done;
    m_buffer_size = buffer_size;

    m_sock_setup.send(m_device_address, Buffer::serialize(request));
}

void DACFront::init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat, OnInitDoneCallback on_init_done)
{
    dac::setup::InitSample request;
    request.timings.period = period;
    request.timings.prescaler = prescaler;
    request.buffer_size = buffer_size;
    request.autorepeat = repeat ? 1 : 0;

    m_on_init_done = on_init_done;
    m_buffer_size = buffer_size;

    m_sock_setup.send(m_device_address, Buffer::serialize(request));
}

void DACFront::send_data(const std::vector<float>& data, OnDataSampleSet on_data_sample_set)
{
/*    if (!m_dac_initialized)
        throw std::runtime_error("DAC is not initialized, cannot send data");

    if (data.size() > m_buffer_size)
        throw std::runtime_error("Data fragment is greater than buffer size");*/

    auto to_send = prepare_to_send(data);

    m_sample = Buffer::create(to_send.size() * sizeof(uint16_t), to_send.data());

    /// TODO Send samples part by part
    m_on_data_sample_set = on_data_sample_set;
    m_data_set_segment_id = m_sock_data.send(m_device_address, m_sample); /// TODO prevent this double copy

}

void DACFront::add_continious_data_chunk(const std::vector<float>& data)
{
}


void DACFront::run(OnRun callback)
{
    dac::setup::RunRequest request;
    request.run_stop = dac::setup::RunRequest::run;
    request.status_reports_receiver = m_host_address;

    m_on_run_callback = callback;
    m_run_segment_id = m_sock_setup.send(m_device_address, Buffer::serialize(request));
}

void DACFront::stop()
{

}

void DACFront::sock_setup_listener()
{
    ISocketUserSide::IncomingMessage incoming = *m_sock_setup.get();
    auto response = try_interpret_buffer_no_magic<dac::setup::InitResponse>(incoming.data);

    if (!response.has_value())
        return;

    if (m_on_init_done)
    {
        m_dac_initialized = response->error_code == 0;
        m_on_init_done(response->error_code);
    }
}

void DACFront::sock_data_lestener()
{
}

void DACFront::sock_setup_on_received_by_device(uint32_t id, bool success)
{
    //std::cout << "Received by device" << std::endl;
    if (id == m_run_segment_id)
    {
        if (m_on_run_callback)
            m_on_run_callback(success);
        m_run_segment_id = 0;
        m_on_run_callback = nullptr;
    }
}

void DACFront::sock_data_on_received(uint32_t id, bool success)
{
    if (id == m_data_set_segment_id)
    {
        if (m_on_data_sample_set)
            m_on_data_sample_set(success ? m_sample->size() : 0);
    }
}


std::vector<uint16_t> DACFront::prepare_to_send(const std::vector<float>& data)
{
    std::vector<uint16_t> to_send(data.size());
    for (size_t i = 0; i < data.size(); i++)
    {
        to_send[i] = uint16_t( round(((1 << 12) - 1) * data[i]));
    }
    return to_send;
}
