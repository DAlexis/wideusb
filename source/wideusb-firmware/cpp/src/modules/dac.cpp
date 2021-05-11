#include "modules/dac.hpp"
#include "communication/modules/dac.hpp"
#include "dac.h"
#include "tim.h"

static DACModule* active_dac_module = nullptr;

DACModule::DACModule(NetSevice& net_service, Address module_address) :
    m_sock_setup(net_service, module_address, dac::setup::port, [this](ISocketUserSide&) { sock_setup_listener(); }),
    m_sock_data(net_service, module_address, dac::data::port, [this](ISocketUserSide&) { sock_data_listener(); })

{
}

DACModule::~DACModule()
{
}

void DACModule::tick()
{
    if (m_ring_buffer != nullptr)
    {
        if (m_ring_buffer->size() <= m_size_limit_notify)
        {
            notify_data_sender();
        }
    }
}

void DACModule::sock_setup_listener()
{
    while (m_sock_setup.has_data())
    {
        Socket::IncomingMessage incoming = *m_sock_setup.get();

        auto init_request = try_interpret_buffer_magic<dac::setup::InitRequest>(incoming.data);
        if (init_request)
        {
            RunMode mode = RunMode::single;
            switch (init_request->mode) {
            case dac::setup::InitRequest::single:
                mode = RunMode::single;
                break;
            case dac::setup::InitRequest::repeat:
                mode = RunMode::repeat;
                break;
            case dac::setup::InitRequest::continious:
                mode = RunMode::continious;
                break;
            }
            uint16_t act_size = init(init_request->buffer_size, init_request->prescaler, init_request->period, mode);

            dac::setup::InitResponse response;
            response.actual_buffer_size = act_size;
            response.success = 1;

            m_sock_setup.send(incoming.sender, Buffer::serialize(response));
        }

        auto run_request = try_interpret_buffer_magic<dac::setup::RunRequest>(incoming.data);
        if (run_request)
        {
            switch(run_request->run_stop)
            {
            case dac::setup::RunRequest::run:
                m_status_reports_receiver = run_request->status_reports_receiver;
                m_size_limit_notify = run_request->size_limit_notify;
                run();
                break;
            default:
            case dac::setup::RunRequest::stop:
                m_stop_now = true;
            };
        }
    }
}

void DACModule::sock_data_listener()
{
    while (m_sock_data.has_data())
    {
        Socket::IncomingMessage incoming = *m_sock_setup.get();
        BufferAccessor acc(incoming.data);

        switch (m_run_mode) {
        case RunMode::continious:
            m_ring_buffer->put(acc, std::min(incoming.data->size(), m_ring_buffer->free_space()));
            break;
        case RunMode::repeat:
        case RunMode::single:
            m_linear_buffer->put(acc, std::min(incoming.data->size(), max_buffer_size - m_linear_buffer->size()));
            break;
        }

        // Data that not fit will be declined
    }
}


uint16_t DACModule::init(uint16_t buffer_size, uint32_t prescaler, uint32_t period, RunMode mode)
{
    m_run_mode = mode;
    buffer_size = std::min(buffer_size, uint16_t(1000));
    switch (m_run_mode) {
    case RunMode::continious:
        m_ring_buffer.reset(new RingBuffer(buffer_size));
        m_linear_buffer.reset();
        break;
    case RunMode::repeat:
    case RunMode::single:
        m_ring_buffer.reset();
        m_linear_buffer = Buffer::create();
    }

    MX_DAC_Init();
    MX_TIM4_Init_Parametric(prescaler, period);
    return buffer_size;
}

void DACModule::play_next_block()
{
    if (m_stop_now)
        return;
    switch (m_run_mode) {
    case RunMode::continious:
        {
            // Check if we have some new data in the buffer. Else previous fragment will be replayed
            if (m_ring_buffer->size() > m_currently_being_played.size)
            {
                m_ring_buffer->skip(m_currently_being_played.size);
            }
            m_currently_being_played = m_ring_buffer->get_continious_block(max_dma_chunk_size);
            HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*) m_currently_being_played.data, m_currently_being_played.size, DAC_ALIGN_12B_L);
            m_already_notified = false;
        }
        break;
    case RunMode::repeat:
        if (m_linear_buffer)
            HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*) m_linear_buffer->data(), m_linear_buffer->size(), DAC_ALIGN_12B_L);
        break;
    case RunMode::single:
        break;
    }
}

void DACModule::run()
{
    active_dac_module = this;
    m_stop_now = false;
    switch (m_run_mode) {
    case RunMode::continious:
        play_next_block();
        break;
    case RunMode::repeat:
    case RunMode::single:
        HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*) m_linear_buffer->data(), m_linear_buffer->size(), DAC_ALIGN_12B_L);
    }
}

void DACModule::stop()
{
    m_stop_now = true;
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
}

void DACModule::notify_data_sender()
{
    /// TODO Potential data race,
    /// put sync from this
    if (m_already_notified)
        return;
    m_already_notified = true;
    /// ^^^ to this
    dac::data::BufferIsShortNotification notification;
    notification.buffer_size = m_ring_buffer->size();
    m_sock_setup.send(m_status_reports_receiver, Buffer::serialize(notification));
}

extern "C" void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
    active_dac_module->play_next_block();
    //HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
}

