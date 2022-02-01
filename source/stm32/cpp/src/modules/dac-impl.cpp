#include "modules/dac-impl.hpp"
#include "dac.h"
#include "tim.h"

static DACImpl* active_dac_module = nullptr;

extern uint16_t test_buffer[];

DACImpl::DACImpl(NetService::ptr net_service, Address module_address) :
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
        if (!m_already_notified)
        {
            notify_data_sender();
            m_already_notified = true;
        }
    }
}

uint16_t DACImpl::init_continious(uint16_t dma_chunk_size, uint32_t prescaler, uint32_t period)
{
    if (dma_chunk_size > max_dma_chunk_size)
        return 2;

    if (m_running)
        return 3;

    m_run_mode = RunMode::continious;
    m_linear_buffer.reset();

    m_continious_buffer_1 = Buffer::create(dma_chunk_size * sizeof(uint16_t));
    m_continious_buffer_2 = Buffer::create(dma_chunk_size * sizeof(uint16_t));

    m_continious_buffer_current = &m_continious_buffer_1;
    m_continious_buffer_next = &m_continious_buffer_2;

    m_buffer_filling_current = 0;
    m_buffer_filling_next = 0;

    m_dma_chunk_size = dma_chunk_size;

    MX_TIM4_Init_Parametric(prescaler, period);
    HAL_TIM_Base_Start(&htim4);
    return 0;
}

uint16_t DACImpl::init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat)
{
    if (buffer_size > 1000)
        return 1;

    if (m_running)
        return 3;

    m_run_mode = repeat ? RunMode::repeat : RunMode::single;

    m_linear_buffer = Buffer::create();
    m_continious_buffer_1.reset();
    m_continious_buffer_2.reset();

    MX_TIM4_Init_Parametric(prescaler, period);
    HAL_TIM_Base_Start(&htim4);
    return 0;
}

void DACImpl::play_next_block()
{
    if (!m_running)
        return;
    switch (m_run_mode) {
    case RunMode::continious:
        play_next_continious_block();
        break;
    case RunMode::repeat:
        HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*) m_linear_buffer->data(), m_linear_buffer->size() / sizeof(uint16_t), DAC_ALIGN_12B_R);
        break;
    case RunMode::single:
        m_running = false;
        break;
    }
}

void DACImpl::play_next_continious_block()
{
    std::swap(m_continious_buffer_next, m_continious_buffer_current);
    m_buffer_filling_current = m_buffer_filling_next;
    m_buffer_filling_next = 0;

    uint32_t dma_data_size = m_buffer_filling_current / 2;
    if (dma_data_size == 0) // If no new data uploaded repeat previous buffer
//        dma_data_size = (*m_continious_buffer_current)->size() / 2;
        dma_data_size = 1; // This helps to inspect buffer issues

    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*) (*m_continious_buffer_current)->data(), dma_data_size, DAC_ALIGN_12B_R);

    m_already_notified = false;
}

void DACImpl::run()
{
    active_dac_module = this;
    m_running = true;
    m_stop_now = false;
    switch (m_run_mode) {
    case RunMode::continious:
        play_next_continious_block();
        break;
    case RunMode::repeat:
    case RunMode::single:
        HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*) m_linear_buffer->data(), m_linear_buffer->size() / sizeof(uint16_t), DAC_ALIGN_12B_R);
    }
}

void DACImpl::stop()
{
    m_running = false;
}

void DACImpl::stop_immediately()
{
    m_running = false;
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
}

void DACImpl::receive_data(PBuffer data)
{
    BufferAccessor acc(data);

    switch (m_run_mode) {
    case RunMode::continious:
        {
            size_t next_buffer_space_left = (*m_continious_buffer_next)->size() - m_buffer_filling_next;
            size_t size_to_copy = std::min(next_buffer_space_left, data->size());
            memcpy(&(**m_continious_buffer_next)[m_buffer_filling_next], data->data(), size_to_copy);
            m_buffer_filling_next += size_to_copy;
            if (m_buffer_filling_next != (*m_continious_buffer_next)->size())
                m_already_notified = false;
        }
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
    send_notification_buffer_is_short((*m_continious_buffer_next)->size() / sizeof(uint16_t));
}

extern "C" void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
    //HAL_DAC_Start_DMA(hdac, DAC_CHANNEL_1, (uint32_t*)test_buffer, sizeof(test_buffer) / 2, DAC_ALIGN_12B_L);
    active_dac_module->play_next_block();
    //HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
}
