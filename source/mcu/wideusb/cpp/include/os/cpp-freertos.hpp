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

namespace os
{

using TaskFunc = std::function<void(void)>;
using Time = uint32_t;
using Priority = int;
using Ticks = uint32_t;
using Handle = void*;

extern const Ticks max_delay;

void delay(Time ms);

class TaskBase
{
public:
    TaskBase(const TaskFunc& task = nullptr, const char* name = "Unknown");
    ~TaskBase();

    inline bool is_running() const;
    inline void delete_after_run(bool doDelete = true);
    inline void set_task(const TaskFunc& _task);
    inline void set_stack_size(uint32_t stackSize);
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

    TaskFunc m_task;
    uint32_t m_stackSize = 128;
    Handle m_taskId = nullptr;
    uint8_t m_state = 0;
    char* m_name;
};

class TaskOnce : public TaskBase
{
public:
    TaskOnce(const TaskFunc& task = nullptr);
    bool run(Time delay = 0, Priority priority = 0);

private:
    Time m_firstRunDelay = 0;
    static void run_task_once(void const* pTask);
};

class TaskCycled : public TaskBase
{
public:
    TaskCycled(const TaskFunc& task = nullptr) :
			TaskBase(task)
	{ }

    bool run(Time firstRunDelay = 0, Time periodMin = 1, Time periodMax = 1, uint32_t cyclesCount = 0);

private:
    Time m_firstRunDelay = 0;
    Time m_periodMin = 0;
    Time m_periodMax = 0;
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

    inline bool lock(Ticks timeout = max_delay);
    inline void unlock();
    inline bool is_locked();
private:

    Handle m_handle;
};

}

#endif // CPP_FREERTOS_HPP_INCLUDED
