#ifndef PRECISION_TIMER_HPP_INCLUDED
#define PRECISION_TIMER_HPP_INCLUDED

#include "tim.h"
#include <functional>

class PrecisionTimer
{
public:
    using SignalCallback = std::function<void(bool has_timing, uint32_t last_second_duration, uint32_t ticks_since_pps)>;
    PrecisionTimer(TIM_HandleTypeDef* tim_handle, SignalCallback signal_callback);
    ~PrecisionTimer();

    uint32_t value();
    void check_for_pps_loss();

    void interrupt_external_signal(uint32_t ticks);
    void interrupt_PPS(uint32_t ticks);
    const TIM_HandleTypeDef* tim_handle();

private:
    constexpr static uint32_t timeout = 80000000; /// @TODO: This value should be corrected

    uint32_t duration(uint32_t from, uint32_t to);

    TIM_HandleTypeDef* m_tim_handle;

    uint32_t m_ticks_prev_pps = 0;
    uint32_t m_ticks_pps = 0;
    uint32_t m_pps_count = 0;

    SignalCallback m_signal_callback = nullptr;
};

#endif // PRECISION_TIMER_HPP_INCLUDED
