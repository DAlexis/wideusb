#include "modules/dac-impl.hpp"
#include "dac.h"
#include "tim.h"

static DACImpl* active_dac_module = nullptr;

extern uint16_t test_buffer[];

DACImpl::DACImpl(NetSevice& net_service, Address module_address) :
    DACModuleBackend(net_service, module_address)

{
    MX_DAC_Init();
}

DACImpl::~DACImpl()
{
}

void DACImpl::tick()
{
    if (m_running && m_run_mode == RunMode::continious)
    {
        if (m_ring_buffer->size() <= m_size_limit_notify)
        {
            notify_data_sender();
        }
    }
}

uint16_t DACImpl::init_continious(uint16_t buffer_size, uint32_t prescaler, uint32_t period, uint16_t dma_chunk_size, uint16_t notify_when_left)
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

uint16_t DACImpl::init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat)
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

void DACImpl::play_next_block()
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

void DACImpl::run()
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

void DACImpl::stop()
{
    m_stop_now = true;
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
}

void DACImpl::receive_data(PBuffer data)
{
    BufferAccessor acc(data);

    switch (m_run_mode) {
    case RunMode::continious:
        m_ring_buffer->put(acc, std::min(data->size(), m_ring_buffer->free_space()));
        break;
    case RunMode::repeat:
    case RunMode::single:
        m_linear_buffer->put(acc, std::min(data->size(), max_buffer_size - m_linear_buffer->size()));
        break;
    }
    // Data that not fit will be declined
}

void DACImpl::notify_data_sender()
{
    /// TODO Potential data race,
    /// put sync from this
    if (m_already_notified)
        return;
    m_already_notified = true;
    /// ^^^ to this

    send_notification_buffer_is_short(m_ring_buffer->size());
}

extern "C" void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
    //HAL_DAC_Start_DMA(hdac, DAC_CHANNEL_1, (uint32_t*)test_buffer, sizeof(test_buffer) / 2, DAC_ALIGN_12B_L);
    active_dac_module->play_next_block();
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
}
