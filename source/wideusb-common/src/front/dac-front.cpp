#include "wideusb/front/dac-front.hpp"

#include "wideusb/communication/modules/dac.hpp"

#include <cmath>

DACFront::DACFront(NetService& host_connection_service, OnModuleCreatedCallbackEntry on_created, Address my_address, Address device_address) :
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
        [this](uint32_t id, bool success) { sock_setup_on_received(id, success); }
    ),
    m_sock_data(
        m_host_connection_service,
        my_address,
        dac::data::port,
        [this](ISocketUserSide&) { sock_data_listener(); },
        [this](uint32_t id, bool success) { sock_data_on_received(id, success); }
    )
{
}

void DACFront::init_continious(uint16_t dma_chunk_size, uint32_t prescaler, uint32_t period, OnInitDoneCallbackEntryPoint on_init_done, OnBufferIsShortEntryPoint on_buffer_short)
{
    dac::setup::InitContinious request;
    request.timings.period = period;
    request.timings.prescaler = prescaler;
    request.chunk_size = dma_chunk_size;

    m_on_init_done = on_init_done;
    m_on_buffer_short = on_buffer_short;

    m_sock_setup.send(m_device_address, Buffer::serialize(request));
}

void DACFront::init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat, OnInitDoneCallbackEntryPoint on_init_done)
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

void DACFront::send_data(const float* data, size_t size, OnDataSampleSetEntryPoint on_data_sample_set)
{
    PBuffer uint16_data = Buffer::create(size*sizeof(uint16_t));

    prepare_to_send(reinterpret_cast<uint16_t*>(uint16_data->data()), data, size);

    m_on_data_sample_set = on_data_sample_set;
    m_data_set_segment_id = m_sock_data.send(m_device_address, uint16_data);
}


void DACFront::run(OnRunEntryPoint on_run)
{
    dac::setup::RunRequest request;
    request.run_stop = dac::setup::RunRequest::run;
    request.status_reports_receiver = m_host_address;

    m_on_run = on_run;
    m_run_segment_id = m_sock_setup.send(m_device_address, Buffer::serialize(request));
}

void DACFront::stop(OnStopEntryPoint on_stop)
{
    dac::setup::RunRequest request;
    request.run_stop = dac::setup::RunRequest::stop;
    request.status_reports_receiver = m_host_address;

    m_on_stop = on_stop;
    m_stop_segment_id = m_sock_setup.send(m_device_address, Buffer::serialize(request));
}

void DACFront::sock_setup_listener()
{
    ISocketUserSide::IncomingMessage incoming = *m_sock_setup.get_incoming();
    auto response = try_interpret_buffer_no_magic<dac::setup::InitResponse>(incoming.data);

    if (!response.has_value())
    {
        return;
    }

    m_dac_initialized = response->error_code == 0;
    m_on_init_done(response->error_code);
}

void DACFront::sock_data_listener()
{
    // @todo TODO May be drop all notification except last?
    while (m_sock_data.has_incoming())
    {
        ISocketUserSide::IncomingMessage incoming = *m_sock_data.get_incoming();
        auto buffer_is_short = try_interpret_buffer_magic<dac::data::BufferIsShortNotification>(incoming.data);
        if (buffer_is_short)
        {
            m_on_buffer_short(buffer_is_short->buffer_size);
        }
    }
}

void DACFront::sock_setup_on_received(uint32_t id, bool success)
{
    //std::cout << "Received by device" << std::endl;
    if (id == m_run_segment_id)
    {
        m_on_run(success);
        m_run_segment_id = 0;
        m_on_run = nullptr;
    }
    else if (id == m_stop_segment_id)
    {
        m_on_stop(success);
        m_stop_segment_id = 0;
        m_on_stop = nullptr;
    }
}

void DACFront::sock_data_on_received(uint32_t id, bool success)
{
    if (id == m_data_set_segment_id)
    {
        m_on_data_sample_set(success);
    }
}

void DACFront::prepare_to_send(uint16_t* dst, const float* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        dst[i] = uint16_t( round(((1 << 12) - 1) * data[i]));
    }
}
