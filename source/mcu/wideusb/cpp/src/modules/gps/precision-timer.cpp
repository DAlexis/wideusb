#include "modules/gps/precision-timer.hpp"

static PrecisionTimer* interrupt_receiver = nullptr;

PrecisionTimer::PrecisionTimer(TIM_HandleTypeDef* tim_handle, SignalCallback signal_callback, PPSCallback pps_callback) :
    m_tim_handle(tim_handle), m_signal_callback(signal_callback), m_pps_callback(pps_callback)
{
    interrupt_receiver = this;
    HAL_TIM_IC_Start_IT(m_tim_handle, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(m_tim_handle, TIM_CHANNEL_2);
}

PrecisionTimer::~PrecisionTimer()
{
    interrupt_receiver = nullptr;
    HAL_TIM_IC_Stop_IT(m_tim_handle, TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(m_tim_handle, TIM_CHANNEL_2);
}

uint32_t PrecisionTimer::value()
{
    return m_tim_handle->Instance->CNT;
}

bool PrecisionTimer::has_pps()
{
    return m_pps_count >= 2;
}

std::optional<float> PrecisionTimer::fract_time()
{
    if (!has_pps())
        return std::nullopt;

    uint32_t second_duration = m_ticks_pps - m_ticks_prev_pps;
    uint32_t ticks_since_pps = value() - m_ticks_pps;

    if (!are_intervals_good_to_get_time(second_duration, ticks_since_pps))
        return std::nullopt;

    return float(ticks_since_pps) / second_duration;
}

void PrecisionTimer::check_for_pps_loss()
{
    // If we have no signal for now
    if (m_pps_count == 0)
        return;

    // Ticks since last GPS signal
    if (value() - m_ticks_pps > timeout)
    {
        m_pps_count = 0;
    }
}

void PrecisionTimer::interrupt_external_signal(uint32_t ticks)
{
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
    if (!has_pps())
    {
        if (m_signal_callback)
            m_signal_callback(false, 0, 0);
        return;
    }

    // We have unsigned arithmetics so we will get always valid positive numbers below
    uint32_t second_duration = m_ticks_pps - m_ticks_prev_pps;
    uint32_t ticks_since_pps = ticks - m_ticks_pps;
    if (are_intervals_good_to_get_time(second_duration, ticks_since_pps)) // @TODO Add correction: timing is valid if ticks_since_pps a bit > than second_duration
    {
        // We have valid time
        if (m_signal_callback)
            m_signal_callback(true, second_duration, ticks_since_pps);
    } else {
        // We had no GPS pulses for a time > 1 sec
        m_pps_count = 0;
        if (m_signal_callback)
            m_signal_callback(false, 0, 0);
    }

}

void PrecisionTimer::interrupt_PPS(uint32_t ticks)
{
    uint32_t second_duration = m_ticks_pps - m_ticks_prev_pps;
    m_ticks_prev_pps = m_ticks_pps;
    m_ticks_pps = ticks;
    m_pps_count++;
    if (m_pps_callback)
        m_pps_callback(second_duration);
}

const TIM_HandleTypeDef* PrecisionTimer::tim_handle()
{
    return m_tim_handle;
}

uint32_t PrecisionTimer::duration(uint32_t from, uint32_t to)
{
    // We assume that time is cycled by max_counter_value
    // This function is not needed when ticks limit is max(uint32_t)
    uint32_t max_counter_value = m_tim_handle->Instance->ARR;
    if (from <= to)
        return to - from;
    else
        return to + max_counter_value - from;
}

bool PrecisionTimer::are_intervals_good_to_get_time(uint32_t second_duration, uint32_t ticks_since_pps)
{
    return ticks_since_pps <= 1.5 * second_duration;
}

extern "C" {
    void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
    {
        if (!interrupt_receiver || htim != interrupt_receiver->tim_handle())
            return;

        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
            interrupt_receiver->interrupt_external_signal(htim->Instance->CCR1);
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
            interrupt_receiver->interrupt_PPS(htim->Instance->CCR2);
    }
}
