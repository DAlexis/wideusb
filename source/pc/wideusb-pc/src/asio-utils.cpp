#include "wideusb-pc/asio-utils.hpp"
#include "wideusb/communication/networking.hpp"

#include <iostream>

Task::Task(boost::asio::io_service& io_service, std::chrono::milliseconds milliseconds, TaskCallback task, OnTaskEndCallback when_task_ends) :
    m_task(task), m_when_task_ends(when_task_ends), m_io_service(io_service), m_interval(milliseconds.count()), m_timer(io_service, m_interval)
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

AsioServiceRunner::AsioServiceRunner(boost::asio::io_service& io_service) :
    m_io_service(io_service)
{
}

AsioServiceRunner::~AsioServiceRunner()
{
    stop_thread();
}

void AsioServiceRunner::run_thread()
{
    m_thread.reset(
        new std::thread(
            [this](){ thread_body(); }
        )
    );
}

void AsioServiceRunner::stop_thread()
{
    m_io_service.stop();
    m_thread->join();
}

void AsioServiceRunner::thread_body()
{
    boost::asio::io_service::work work(m_io_service);
    m_io_service.run();
}

std::shared_ptr<IOServiceRunner> IOServiceRunner::create()
{
    return std::shared_ptr<IOServiceRunner>(new IOServiceRunner());
}

IOServiceRunner::IOServiceRunner() :
    m_work(std::make_shared<boost::asio::io_service::work>(m_io_service)),
    m_service_thread([this]() { m_io_service.run(); std::cout << "Thread done." << std::endl; })
{
}

IOServiceRunner::~IOServiceRunner()
{
    stop();
}

boost::asio::io_service& IOServiceRunner::io_service()
{
    return m_io_service;
}

void IOServiceRunner::stop()
{
    m_work.reset();
    m_io_service.stop();
    m_service_thread.join();
}

void IOServiceRunner::join()
{
    m_service_thread.join();
}

NetworkManagerPlaner::NetworkManagerPlaner(std::shared_ptr<IOServiceRunner> runner, NetService& net_srv, std::chrono::milliseconds serve_tick_delay) :
    m_runner(runner), m_net_srv(net_srv),
    m_serve_sockets_task(runner->io_service(), serve_tick_delay,
    [this]() {
        m_net_srv.serve_sockets(std::chrono::steady_clock::now());
        return true;
    })
{

}

NetworkManagerPlaner::~NetworkManagerPlaner()
{
    m_serve_sockets_task.cancel();
}

void NetworkManagerPlaner::post_serve_sockets()
{
    boost::asio::post(m_runner->io_service(), [this]() { m_net_srv.serve_sockets(std::chrono::steady_clock::now()); });
}
