/*
 * cpp-freertos.hpp
 *
 * C++ FreeRTOS wrapper without dependency from cmsis_os.h header
 *
 *  Created on:
 *      Author: dalexies
 */

#ifndef CPP_FREERTOS_HPP_INCLUDED
#define CPP_FREERTOS_HPP_INCLUDED

#include "os/os-types.hpp"

#include <optional>
#include <functional>
#include <memory>

#include <stdint.h>
#include <vector>
#include <chrono>

using namespace std::literals::chrono_literals;

namespace os
{

#ifdef DEBUG
    #define ASSERT(condition, message) os::assert(message, __FILE__, __LINE__)
#else
    #define ASSERT(condition, message)
#endif


extern const Ticks max_delay;

void delay(std::chrono::steady_clock::duration duration);
void delay_iter_us(uint32_t count);
void delay_spinlock(std::chrono::steady_clock::duration duration);

void assert_print(const char* message, const char* file, int line);

bool is_inside_interrupt();

class Thread
{
public:
    enum class Priority {
        idle         = -3,          ///< priority: idle (lowest)
        low          = -2,          ///< priority: low
        below_normal = -1,          ///< priority: below normal
        normal       =  0,          ///< priority: normal (default)
        above_normal = +1,          ///< priority: above normal
        high         = +2,          ///< priority: high
        realtime     = +3,          ///< priority: realtime (highest)
    };

    Thread(TaskFunction function, const char* name = "Unknown", uint32_t stack_size = 128, Priority priority = Priority::normal);

    Handle handle();
    void run();

    void notify_give_form_ISR();
    void notify_give();

    static void yeld();

    /**
     * @brief Wait for notification in thread from which this function was called.
     * @param clean    Clean notification after taking, so next take will need next notification.
     * @param timeout  Timeout in ticks.
     * @return Count of notifications before ot was cleaned. Zero if timeouted.
     */
    static uint32_t notify_take(bool clean = true, Ticks timeout = max_delay);

private:
    void thread_body();
    static void thread_body_bootstrap(const void * thread_object);

    const char* m_name;
    uint32_t m_stack_size;
    Priority m_priority;
    bool m_is_running = false;
    Handle m_task_handle = nullptr;
    TaskFunction m_task_function = nullptr;
};

class TaskBase
{
public:
    TaskBase(const TaskFunction& task, const char* name = "Unknown");
    ~TaskBase();

    bool is_running() const;
    void delete_after_run(bool doDelete = true);
    void set_task(const TaskFunction& _task);
    void set_stack_size(uint32_t stackSize);
    void set_name(const char* name);

	/**
	 * Stop task safely, all destructors will be called
	 * @return is task executing after this call
	 */
    bool stop_safe();
	/**
	 * Call vTaskDelete. Attention! This operation immediately stop the task
	 * without calling any destructors
	 */
    void stop_unsafe();

protected:
    constexpr static uint8_t running_now = 1;
    constexpr static uint8_t need_delete_self = 2;
    constexpr static uint8_t need_stop = 4;
    constexpr static uint8_t executing_now = 8;

    void stop_thread();

    TaskFunction m_task;
    uint32_t m_stackSize = 128;
    Handle m_taskId = nullptr;
    uint8_t m_state = 0;
    char* m_name;
};

class TaskOnce : public TaskBase
{
public:
    TaskOnce(const TaskFunction& task = nullptr);
    bool run(Time_ms delay = 0, Priority priority = 0);

private:
    Time_ms m_firstRunDelay = 0;
    static void run_task_once(void const* pTask);
};

class TaskCycled : public TaskBase
{
public:
    TaskCycled(const TaskFunction& task = nullptr, const char* name = "Unknown_cycled");

    bool run(Time_ms period = 0, Time_ms firstRunDelay = 0, uint32_t cyclesCount = 0);

private:
    Time_ms m_firstRunDelay = 0;
    Time_ms m_period = 0;
	uint32_t m_cyclesCount = 0;
    static void run_task_in_cycle(void const* pTask);
};

class Mutex
{
public:
    Mutex();
    ~Mutex();

    bool lock(Ticks timeout);
    void lock();
    void unlock();
    bool is_locked();
private:

    Handle m_handle;
};

class CriticalSection
{
public:
    CriticalSection();
    ~CriticalSection();

    void unlock();
    bool is_locked();

private:
    bool m_locked = true;
};

class CriticalSectionISR
{
public:
    CriticalSectionISR();
    ~CriticalSectionISR();

    void unlock();
    bool is_locked();

private:
    bool m_locked = true;
    unsigned long m_uxSavedInterruptStatus = 0;
};


class QueueBase
{
public:
    QueueBase(unsigned int queue_size, size_t object_size);
    ~QueueBase();

    bool push_back(const void* obj, Ticks time_to_wait = max_delay);
    bool push_front(const void* obj, Ticks time_to_wait = max_delay);
    bool front(void* target, Ticks time_to_wait = max_delay);
    bool pop_front(void* target, Ticks time_to_wait = max_delay);
    bool push_back_from_ISR(const void* obj);
    bool push_front_from_ISR(const void* obj);
    bool pop_front_from_ISR(void* target);

    uint16_t size();
    uint16_t size_from_ISR();
    bool empty();
    bool empty_from_ISR();
    size_t capacity();
    size_t space();

protected:
    size_t m_capacity;

private:
    Handle m_handle;
};


template<typename T>
class QueueCopying : public QueueBase
{
public:
    QueueCopying(unsigned int size = 10) :
        QueueBase(size, sizeof(T))
    { }

    bool push_back(const T& obj, Ticks time_to_wait = max_delay)
	{
        return QueueBase::push_back(&obj, time_to_wait);
	}

    bool push_front(const T& obj, Ticks time_to_wait = max_delay)
	{
        return QueueBase::push_front(&obj, time_to_wait);
	}

    std::optional<T> pop_front(Ticks time_to_wait = max_delay)
	{
        T target;
        if (QueueBase::pop_front(&target, time_to_wait))
            return target;
        else
            return std::nullopt;
	}

    std::optional<T> front(Ticks time_to_wait = max_delay)
    {
        T target;
        if (QueueBase::front(&target, time_to_wait))
            return target;
        else
            return std::nullopt;
    }

    void push_back_from_ISR(const T& obj)
	{
        QueueBase::push_back_from_ISR(&obj);
	}

    void push_front_from_ISR(const T& obj)
	{
        QueueBase::push_front_from_ISR(&obj);
	}

    std::optional<T> pop_front_from_ISR()
    {
        T target;
        if (QueueBase::pop_front_from_ISR(&target))
            return target;
        else
            return std::nullopt;
	}
};

template<typename T>
class QueuePointerBased : protected QueueBase
{
public:
    QueuePointerBased(unsigned int size = 10) :
        QueueBase(size, sizeof(T*))
    { }

    bool push_back(const T& obj)
    {
        T* p_copy = new T{obj};
        if (is_inside_interrupt())
            return QueueBase::push_back_from_ISR(&p_copy);
        else
            return QueueBase::push_back(&p_copy, 0);
    }

    bool push_front(const T& obj, Ticks time_to_wait = max_delay)
    {
        T* p_copy = new T{obj};
        if (is_inside_interrupt())
            return QueueBase::push_front_from_ISR(&p_copy);
        else
            return QueueBase::push_front(&p_copy, time_to_wait);
    }

    std::optional<T> pop_front(Ticks time_to_wait = max_delay)
    {
        T* p_data;
        if (is_inside_interrupt())
        {
            if (!QueueBase::pop_front_from_ISR(&p_data))
                return std::nullopt;
        } else {
            if (!QueueBase::pop_front(&p_data, time_to_wait))
                return std::nullopt;
        }
        T result(*p_data);

        delete p_data;
        return result;
    }

    std::optional<T> front(Ticks time_to_wait = max_delay)
    {
        T* p_data;
        if (!QueueBase::front(&p_data, time_to_wait))
        {
            return std::nullopt;
        }
        return *p_data;
    }

    using QueueBase::capacity;
    using QueueBase::size;
    using QueueBase::space;
};

}

#endif // CPP_FREERTOS_HPP_INCLUDED
