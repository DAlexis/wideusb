#include "asio-utils.hpp"

#include <iostream>

Task::Task(boost::asio::io_service& io_service, size_t milliseconds, TaskCallback task, OnTaskEndCallback when_task_ends) :
    m_task(task), m_when_task_ends(when_task_ends), m_io_service(io_service), m_interval(milliseconds), m_timer(io_service, m_interval)
{
    post_task();
}

void Task::cancel()
{
    //std::cout << "Canceling" << std::endl;
    m_canceled = true;
    m_timer.cancel();
}

void Task::on_timer_done()
{
    //std::cout << "on timer done" << std::endl;
    if (m_canceled)
        return;

    if (m_task())
    {
        if (m_when_task_ends)
            m_when_task_ends();
        return;
    }

    post_task();
}

void Task::post_task()
{
    m_timer.expires_from_now(m_interval);
    m_timer.async_wait([this](const boost::system::error_code&){ on_timer_done(); });
}


DeferredTask::DeferredTask(boost::asio::io_service& io_service, size_t milliseconds, DeferredTaskCallback callback) :
    m_callback(callback), m_interval(milliseconds), m_timer(io_service, m_interval)
{
    m_timer.expires_from_now(m_interval);
    m_timer.async_wait([this](const boost::system::error_code&){ on_timer_done(); });
}

void DeferredTask::set_self_shared_ptr(std::shared_ptr<DeferredTask> this_object)
{
    m_this_object = this_object;
}

void DeferredTask::on_timer_done()
{
    m_callback();
    m_this_object.reset();
}

void DeferredTask::run(boost::asio::io_service& io_service, size_t milliseconds, DeferredTaskCallback callabck)
{
    auto obj = std::shared_ptr<DeferredTask>(new DeferredTask(io_service, milliseconds, callabck));
    obj->set_self_shared_ptr(obj);
}
