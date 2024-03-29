/*
 * os-wrappers.cpp
 *
 *  Created on: 7 дек. 2016 г.
 *      Author: dalexies
 */

#include "os/cpp-freertos.hpp"
#include "macro.hpp"

#include "cmsis_os.h"

#include <stdio.h>
#include <cstring>

extern "C" void __attribute__((naked)) sys_ctl_dalay(unsigned long ulCount)
{
    __asm("    subs    r0, #1\n"
       "    bne     sys_ctl_dalay\n"
       "    bx      lr");
}

namespace os {

const Ticks max_delay = portMAX_DELAY;

void delay_iter_us(uint32_t count)
{
    sys_ctl_dalay(count * 168 / 3);
}

void assert_print(const char* message, const char* file, int line)
{
    printf("Assertion %s at %s:%d\r\n", message, file, line);
}

bool is_inside_interrupt()
{
    return pdTRUE == xPortIsInsideInterrupt();
}

void delay(std::chrono::steady_clock::duration duration)
{
    osDelay( std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() );
}

void delay_spinlock(std::chrono::steady_clock::duration duration)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - begin < duration)
    {
        // nothing
    }
}

Ticks get_ticks_count()
{
    return xTaskGetTickCount();
}

Thread::Thread(TaskFunction function, const char* name, uint32_t stack_size, Priority priority) :
    m_name(name), m_stack_size(stack_size), m_priority(priority), m_task_function(function)
{
}

Handle Thread::handle()
{
    return m_task_handle;
}

void Thread::run()
{
    os_thread_def threadDef;
    memset(&threadDef, 0, sizeof(threadDef));
    threadDef.name = const_cast<char*>(m_name);
    threadDef.pthread = thread_body_bootstrap;
    threadDef.tpriority = (osPriority) m_priority;
    threadDef.instances = 0;
    threadDef.stacksize = m_stack_size;

    m_is_running = true;
    m_task_handle = osThreadCreate(&threadDef, reinterpret_cast<void*>(this));
}

uint32_t Thread::notify_take(bool clean, Ticks timeout)
{
    return ulTaskNotifyTake(clean ? pdTRUE : pdFALSE, TickType_t(timeout));
}

void Thread::notify_give_form_ISR()
{
    /// https://www.freertos.org/RTOS_Task_Notification_As_Binary_Semaphore.html
    if (!m_is_running)
        return;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR((TaskHandle_t) m_task_handle, &xHigherPriorityTaskWoken);

    /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
    should be performed to ensure the interrupt returns directly to the highest
    priority task.  The macro used for this purpose is dependent on the port in
    use and may be called portEND_SWITCHING_ISR(). */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void Thread::notify_give()
{
    xTaskNotifyGive((TaskHandle_t) m_task_handle);
}

void Thread::yeld()
{
    taskYIELD();
}

void Thread::thread_body()
{
    m_task_function();
    m_is_running = false;
}

void Thread::thread_body_bootstrap(const void * thread_object)
{
    reinterpret_cast<Thread *>(const_cast<void*>(thread_object))->thread_body();
}

TaskBase::TaskBase(const TaskFunction& task, const char* name) :
    m_task(task),
    m_name(const_cast<char*>(name))
{ }

TaskBase::~TaskBase() = default; // This is done to use unique_ptr with forward declaration

bool TaskBase::is_running() const
{
    return m_state & running_now;
}

void TaskBase::delete_after_run(bool doDelete)
{
    if (doDelete)
        m_state |= need_delete_self;
    else
        m_state &= ~(need_delete_self);
}

void TaskBase::set_task(const TaskFunction& _task)
{
    m_task = _task;
}

void TaskBase::set_stack_size(uint32_t stackSize)
{
    m_stackSize = stackSize;
}

void TaskBase::set_name(const char* name)
{
    m_name = const_cast<char*>(name);
}

bool TaskBase::stop_safe()
{
    if (!(m_state & running_now))
        return true;

    if (m_state & executing_now)
    {
        ADD_BITS(m_state, need_stop);
        return false;
    } else {
        vTaskDelete( (osThreadId) m_taskId);
        m_taskId = nullptr;
        REMOVE_BITS(m_state, running_now);
        REMOVE_BITS(m_state, need_stop);
        return true;
    }
}

void TaskBase::stop_unsafe()
{
    if (m_taskId)
    {
        vTaskDelete((osThreadId) m_taskId);
        m_taskId = nullptr;
    }
}


void TaskBase::stop_thread()
{
    REMOVE_BITS(m_state, running_now);
    REMOVE_BITS(m_state, need_stop);
    m_taskId = nullptr;
    vTaskDelete(NULL);
}


void TaskOnce::run_task_once(void const* pTask)
{
	TaskOnce *task = reinterpret_cast<TaskOnce*> (const_cast<void*>(pTask));
	osDelay(task->m_firstRunDelay);

    ADD_BITS(task->m_state, executing_now);
    task->m_task();

    if (task->m_state & need_delete_self)
	{
		delete task;
		vTaskDelete(NULL);
	} else {
        task->stop_thread();
	}
}

void TaskCycled::run_task_in_cycle(void const* pTask)
{
	TaskCycled *task = reinterpret_cast<TaskCycled*> (const_cast<void*>(pTask));
	osDelay(task->m_firstRunDelay);
	if (task->m_cyclesCount == 0)
	{
		for (;;)
		{
            ADD_BITS(task->m_state, executing_now);
            task->m_task();
            REMOVE_BITS(task->m_state, executing_now);
            if (task->m_state & need_stop)
			{
                task->stop_thread();
				return;
			}
            if (task->m_period == 0)
			{
				taskYIELD();
			} else {
                osDelay(task->m_period);
			}
		}
	} else {
		for (uint32_t i=0; i<task->m_cyclesCount;i++)
		{
            ADD_BITS(task->m_state, executing_now);
            task->m_task();
            REMOVE_BITS(task->m_state, executing_now);
            if (task->m_state & need_stop)
			{
                task->stop_thread();
				return;
			}
            if (task->m_period == 0)
			{
				taskYIELD();
			} else {
                osDelay(task->m_period);
			}
		}
        task->stop_thread();
	}
}

TaskOnce::TaskOnce(const TaskFunction& task) :
    TaskBase(task)
{ }

bool TaskOnce::run(Time_ms delay, Priority priority)
{
    ADD_BITS(m_state, running_now);
	m_firstRunDelay = delay;
	//osThreadDef(newTask, runTaskOnce, priority, 0, m_stackSize);

	os_thread_def threadDef;
    threadDef.name = m_name;
    threadDef.pthread = run_task_once;
    threadDef.tpriority = (osPriority) priority;
	threadDef.instances = 0;
    threadDef.stacksize = m_stackSize;

    m_taskId = osThreadCreate(&threadDef, reinterpret_cast<void*>(this));
    if (m_taskId == NULL)
	{
        REMOVE_BITS(m_state, running_now);
		printf("ERROR: Running new task failed!\n");
		return false;
	}
	return true;
}

TaskCycled::TaskCycled(const TaskFunction& task, const char* name) :
    TaskBase(task, name)
{ }

bool TaskCycled::run(Time_ms period, Time_ms firstRunDelay, uint32_t cyclesCount)
{
    ADD_BITS(m_state, running_now);
	m_firstRunDelay = firstRunDelay;
    m_period = period;
	m_cyclesCount = cyclesCount;
	//osThreadDef(newTask, runTaskInCycle, osPriorityNormal, 0, m_stackSize);

	os_thread_def threadDef;
    threadDef.name = m_name;
    threadDef.pthread = run_task_in_cycle;
	threadDef.tpriority = osPriorityNormal;
	threadDef.instances = 0;
    threadDef.stacksize = m_stackSize;

    m_taskId = osThreadCreate(&threadDef, reinterpret_cast<void*>(this));
    if (m_taskId == NULL)
	{
        REMOVE_BITS(m_state, running_now);
		return false;
	}
	return true;
}

QueueBase::QueueBase(unsigned int queue_size, size_t object_size) :
    m_capacity(queue_size), m_handle((Handle) xQueueCreate(queue_size, object_size))
{
}

QueueBase::~QueueBase()
{
    vQueueDelete((QueueHandle_t) m_handle);
}

bool QueueBase::push_back(const void* obj, Ticks time_to_wait)
{
    return xQueueSendToBack((xQueueHandle) m_handle, obj, time_to_wait) == pdTRUE;
}

bool QueueBase::push_front(const void* obj, Ticks time_to_wait)
{
    return xQueueSendToFront((xQueueHandle) m_handle, obj, time_to_wait) == pdTRUE;
}

bool QueueBase::front(void* target, Ticks time_to_wait)
{
    return xQueuePeek((xQueueHandle) m_handle, target, time_to_wait) == pdTRUE;
}

bool QueueBase::pop_front(void* target, Ticks time_to_wait)
{
    return xQueueReceive((xQueueHandle) m_handle, target, time_to_wait) == pdTRUE;
}

bool QueueBase::push_back_from_ISR(const void* obj)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    bool result = xQueueSendToBackFromISR((xQueueHandle) m_handle, obj, &xHigherPriorityTaskWoken) == pdPASS;
    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    return result;
}

bool QueueBase::push_front_from_ISR(const void* obj)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    bool result = xQueueSendToFrontFromISR((xQueueHandle) m_handle, obj, &xHigherPriorityTaskWoken) == pdPASS;
    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    return result;
}

bool QueueBase::pop_front_from_ISR(void* target)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    bool result = xQueueReceiveFromISR((xQueueHandle) m_handle, target, &xHigherPriorityTaskWoken) == pdPASS;
    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    return result;
}

uint16_t QueueBase::size()
{
    return uxQueueMessagesWaiting((xQueueHandle) m_handle);
}

uint16_t QueueBase::size_from_ISR()
{
    return uxQueueMessagesWaitingFromISR((xQueueHandle) m_handle);
}

bool QueueBase::empty()
{
    return size() == 0;
}

bool QueueBase::empty_from_ISR()
{
    return size_from_ISR() == 0;
}

size_t QueueBase::capacity()
{
    return m_capacity;
}

size_t QueueBase::space()
{
    return uxQueueSpacesAvailable((xQueueHandle) m_handle);
}

Mutex::Mutex() :
    m_handle(xSemaphoreCreateMutex())
{
}

Mutex::~Mutex()
{
    if (m_handle) vSemaphoreDelete((xSemaphoreHandle) m_handle);
}

bool Mutex::lock(Ticks timeout)
{
    return (xSemaphoreTake((xSemaphoreHandle) m_handle, timeout ) == pdTRUE);
}

void Mutex::lock()
{
    lock(max_delay);
}

void Mutex::unlock()
{
    xSemaphoreGive((xSemaphoreHandle) m_handle );
}

bool Mutex::is_locked()
{
    bool wasNotLocked = lock(0);
    if (wasNotLocked) {
        unlock();
        return false;
    }
    return true;
}

CriticalSection::CriticalSection()
{
    taskENTER_CRITICAL();
}

CriticalSection::~CriticalSection()
{
    unlock();
}

void CriticalSection::unlock()
{
    if (m_locked)
    {
        taskEXIT_CRITICAL();
        m_locked = false;
    }
}

bool CriticalSection::is_locked()
{
    return m_locked;
}

CriticalSectionISR::CriticalSectionISR()
{
    m_uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
}

CriticalSectionISR::~CriticalSectionISR()
{
    unlock();
}

void CriticalSectionISR::unlock()
{
    if (m_locked)
    {
        taskEXIT_CRITICAL_FROM_ISR(m_uxSavedInterruptStatus);
        m_locked = false;
    }
}

bool CriticalSectionISR::is_locked()
{
    return m_locked;
}

}

///////////////////////////////////////
/// Global new/delete redefinition
/*
void * operator new(std::size_t n)
{
    return pvPortMalloc(n);
}

void operator delete(void * p) noexcept
{
    vPortFree(p);
}

void operator delete(void * p, size_t)
{
    vPortFree(p);
}

void *operator new[](std::size_t n)
{
    return pvPortMalloc(n);
}

void operator delete[](void *p) throw()
{
    vPortFree(p);
}

void operator delete[](void *p, size_t) throw()
{
    vPortFree(p);
}
*/
