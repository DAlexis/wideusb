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

#include <functional>
#include <memory>

#include <stdint.h>
#include <vector>

namespace os
{

#ifdef DEBUG
    #define ASSERT(condition, message) os::assert(message, __FILE__, __LINE__)
#else
    #define ASSERT(condition, message)
#endif

using TaskFunction = std::function<void(void)>;
using Time_ms = uint32_t;
using Priority = int;
using Ticks = uint32_t;
using Handle = void*;

extern const Ticks max_delay;

Time_ms get_os_time();
void delay(Time_ms ms);

void assert_print(const char* message, const char* file, int line);

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

class QueueBase
{
public:
    QueueBase(unsigned int queue_size, size_t object_size);
    ~QueueBase();

    void push_back(const void* obj, Ticks time_to_wait = max_delay);
    void push_front(const void* obj, Ticks time_to_wait = max_delay);
    void pop_front(void* target, Ticks time_to_wait = max_delay);
    void push_back_from_ISR(const void* obj);
    void push_front_from_ISR(const void* obj);
    void pop_front_from_ISR(void* target);

    uint16_t size();
    uint16_t size_from_ISR();
    bool empty();
    bool empty_from_ISR();


private:
    Handle m_handle;
};

template<typename T>
class Queue : public QueueBase
{
public:
    Queue(unsigned int size = 10) :
        QueueBase(size, sizeof(T))
    { }

    void push_back(const T& obj, Ticks time_to_wait = max_delay)
	{
        QueueBase::push_back(&obj, time_to_wait);
	}

    void push_front(const T& obj, Ticks time_to_wait = max_delay)
	{
        QueueBase::push_front(&obj, time_to_wait);
	}

    void pop_front(T& target, Ticks time_to_wait = max_delay)
	{
        QueueBase::pop_front(&target, time_to_wait);
	}

    void push_back_from_ISR(const T& obj)
	{
        QueueBase::push_back_from_ISR(&obj);
	}

    void push_front_from_ISR(const T& obj)
	{
        QueueBase::push_front_from_ISR(&obj);
	}

    void pop_front_from_ISR(T& target)
    {
        QueueBase::pop_front_from_ISR(&target);
	}
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

}

#endif // CPP_FREERTOS_HPP_INCLUDED
