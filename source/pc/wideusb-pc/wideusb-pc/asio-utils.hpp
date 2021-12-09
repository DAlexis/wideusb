#ifndef ASIO_TASK_HPP
#define ASIO_TASK_HPP

#include <boost/asio.hpp>
#include <chrono>
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

class WaiterBase
{
protected:
    void wait_for_notification()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_task_done)
        {
            if (m_cv.wait_for(lock, std::chrono::seconds(1)) == std::cv_status::timeout)
            {
                throw std::runtime_error("Operation timeouted");
            }
        }
    }

    void notify()
    {
        m_task_done = true;
        m_cv.notify_all();
    }

    bool m_task_done = false;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

template<typename CallbackArgType>
class Waiter : public WaiterBase
{
public:
    std::function<void(CallbackArgType)> get_waiter_callback()
    {
        return [this](CallbackArgType arg){ m_callback_arg = arg; notify(); };
    }

    CallbackArgType wait()
    {
        wait_for_notification();
        return m_callback_arg;
    }

private:

    CallbackArgType m_callback_arg;
};

template<>
class Waiter<void> : public WaiterBase
{
public:
    std::function<void(void)> get_waiter_callback()
    {
        return [this](){ notify(); };
    }

    void wait()
    {
        wait_for_notification();
    }
};

class AsioServiceRunner
{
public:
    AsioServiceRunner(boost::asio::io_service& io_service);
    ~AsioServiceRunner();

    void run_thread();
    void stop_thread();

private:
    void thread_body();
    boost::asio::io_service& m_io_service;
    std::unique_ptr<std::thread> m_thread;
};

class IOServiceRunner
{
public:
    IOServiceRunner();
    ~IOServiceRunner();

    boost::asio::io_service& io_service();

    /**
     * @brief Release io_service::work object, stop service and join worker thread
     */
    void stop();

    /**
     * @brief Join the worker thread
     */
    void join();
private:

    boost::asio::io_service m_io_service;
    std::shared_ptr<boost::asio::io_service::work> m_work;
    std::thread m_service_thread;
};

#endif // ASIO_TASK_HPP
