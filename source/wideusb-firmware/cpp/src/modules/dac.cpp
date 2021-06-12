#include "modules/dac.hpp"
#include "communication/modules/dac.hpp"
#include "dac.h"
#include "tim.h"

static DACModule* active_dac_module = nullptr;

extern uint16_t test_buffer[];

DACModule::DACModule(NetSevice& net_service, Address module_address) :
    m_sock_setup(net_service, module_address, dac::setup::port, [this](ISocketUserSide&) { sock_setup_listener(); }),
    m_sock_data(net_service, module_address, dac::data::port, [this](ISocketUserSide&) { sock_data_listener(); })

{
    MX_DAC_Init();
}

DACModule::~DACModule()
{
}

void DACModule::tick()
{
    if (m_running && m_run_mode == RunMode::continious)
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
        auto req_init_continious = try_interpret_buffer_magic<dac::setup::InitContinious>(incoming.data);
        if (req_init_continious)
        {
            uint16_t error_code = init_continious(
                        req_init_continious->buffer_size,
                        req_init_continious->timings.prescaler,
                        req_init_continious->timings.period,
                        req_init_continious->chunk_size,
                        req_init_continious->notify_when_left);

            dac::setup::InitResponse response;
            response.error_code = error_code;

            m_sock_setup.send(incoming.sender, Buffer::serialize(response));
            continue;
        }

        auto req_init_sample = try_interpret_buffer_magic<dac::setup::InitSample>(incoming.data);
        if (req_init_sample)
        {
            uint16_t error_code = init_sample(
                        req_init_sample->buffer_size,
                        req_init_sample->timings.prescaler,
                        req_init_sample->timings.period,
                        req_init_sample->autorepeat != 0);

            dac::setup::InitResponse response;
            response.error_code = error_code;

            m_sock_setup.send(incoming.sender, Buffer::serialize(response));
            continue;
        }

        auto req_run_stop = try_interpret_buffer_magic<dac::setup::RunRequest>(incoming.data);
        if (req_run_stop)
        {
            if (req_run_stop->run_stop == dac::setup::RunRequest::run)
            {
                m_status_reports_receiver = req_run_stop->status_reports_receiver;
                run();
            } else if (req_run_stop->run_stop == dac::setup::RunRequest::stop)
            {
                stop();
            }
            continue;
        }

    }
}

void DACModule::sock_data_listener()
{
    while (m_sock_data.has_data())
    {
        Socket::IncomingMessage incoming = *m_sock_data.get();
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


uint16_t DACModule::init_continious(uint16_t buffer_size, uint32_t prescaler, uint32_t period, uint16_t dma_chunk_size, uint16_t notify_when_left)
{
    if (buffer_size > 1000)
        return 1;

    if (dma_chunk_size > max_dma_chunk_size)
        return 2;

    if (m_running)
        return 3;

    m_run_mode = RunMode::continious;
    m_linear_buffer.reset();
    m_ring_buffer.reset(new RingBuffer(buffer_size));

    m_dma_chunk_size = dma_chunk_size;
    m_size_limit_notify = notify_when_left;

    MX_DAC_Init();
    MX_TIM4_Init_Parametric(prescaler, period);
    return 0;
}

uint16_t DACModule::init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat)
{
    if (buffer_size > 1000)
        return 1;

    if (m_running)
        return 3;

    m_run_mode = repeat ? RunMode::repeat : RunMode::single;

    m_ring_buffer.reset();
    m_linear_buffer = Buffer::create();

    MX_TIM4_Init_Parametric(prescaler, period);
    HAL_TIM_Base_Start(&htim4);

    //MX_DAC_Init();
    //MX_TIM4_Init();
    //MX_TIM4_Init_Parametric(prescaler, period);
    return 0;
}

void DACModule::play_next_block()
{
    /*HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)test_buffer, 100, DAC_ALIGN_12B_L);
    return;*/
    if (m_stop_now)
    {
        m_running = false;
        return;
    }
    switch (m_run_mode) {
    case RunMode::continious:
        {
            // Check if we have some new data in the buffer. Else previous fragment will be replayed
            if (m_ring_buffer->size() > m_currently_being_played.size)
            {
                m_ring_buffer->skip(m_currently_being_played.size);
            }
            m_currently_being_played = m_ring_buffer->get_continious_block(max_dma_chunk_size);
            HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*) m_currently_being_played.data, m_currently_being_played.size / 2, DAC_ALIGN_12B_L);
            m_already_notified = false;
        }
        break;
    case RunMode::repeat:
        HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*) m_linear_buffer->data(), m_linear_buffer->size() / 2, DAC_ALIGN_12B_L);
        break;
    case RunMode::single:
        m_running = false;
        break;
    }
}

void DACModule::run()
{
    active_dac_module = this;
    m_running = true;
    m_stop_now = false;
    switch (m_run_mode) {
    case RunMode::continious:
        play_next_block();
        break;
    case RunMode::repeat:
    case RunMode::single:
        HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*) m_linear_buffer->data(), m_linear_buffer->size() / 2, DAC_ALIGN_12B_L);
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
    //HAL_DAC_Start_DMA(hdac, DAC_CHANNEL_1, (uint32_t*)test_buffer, sizeof(test_buffer) / 2, DAC_ALIGN_12B_L);
    active_dac_module->play_next_block();
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
}


