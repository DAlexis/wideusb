#ifndef ASIO_TASK_HPP
#define ASIO_TASK_HPP

#include <boost/asio.hpp>
#include <memory>

class Task
{
public:
    using TaskCallback = std::function<bool(void)>;
    using OnTaskEndCallback = std::function<void(void)>;

    Task(boost::asio::io_service& io_service, size_t milliseconds, TaskCallback task, OnTaskEndCallback when_task_ends = nullptr);

    void cancel();

private:
    void on_timer_done();
    void post_task();

    bool m_canceled = false;
    TaskCallback m_task;
    OnTaskEndCallback m_when_task_ends;

    boost::asio::io_service& m_io_service;
    boost::posix_time::milliseconds m_interval;
    boost::asio::deadline_timer m_timer;
};

class DeferredTask
{
public:
    using DeferredTaskCallback = std::function<void(void)>;
    static void run(boost::asio::io_service& io_service, size_t milliseconds, DeferredTaskCallback callabck);

private:
    DeferredTask(boost::asio::io_service& io_service, size_t milliseconds, DeferredTaskCallback callabck);
    void on_timer_done();
    void set_self_shared_ptr(std::shared_ptr<DeferredTask> this_object);

    DeferredTaskCallback m_callback;
    std::shared_ptr<DeferredTask> m_this_object;
    boost::posix_time::milliseconds m_interval;
    boost::asio::deadline_timer m_timer;
};

#endif // ASIO_TASK_HPP
