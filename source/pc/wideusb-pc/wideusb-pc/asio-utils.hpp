#ifndef ASIO_TASK_HPP
#define ASIO_TASK_HPP

#include "wideusb/utils/utilities.hpp"
#include <boost/asio.hpp>
#include <chrono>
#include <memory>
#include <type_traits>

template<typename CallbackArgType>
class Waiter
{
public:
    using ArgStorageType = typename std::decay<CallbackArgType>::type;

    Waiter()
    {
        m_callback_receiver = CallbackReceiver<CallbackArgType>::create([this](CallbackArgType arg) { m_promise.set_value(arg); });
    }

    std::shared_ptr<CallbackReceiver<CallbackArgType>> receiver() { return m_callback_receiver; }

    ArgStorageType wait(std::chrono::milliseconds timeout)
    {
        if (m_future.wait_for(timeout) != std::future_status::ready)
            throw std::runtime_error("Operation timeouted");

        return m_future.get();
    }


private:
    std::shared_ptr<CallbackReceiver<CallbackArgType>> m_callback_receiver;
    std::promise<ArgStorageType> m_promise;
    std::future<ArgStorageType> m_future{m_promise.get_future()};
};

template<>
class Waiter<void>
{
public:
    Waiter()
    {
        m_callback_receiver = CallbackReceiver<void>::create([this]() { m_promise.set_value(); });
    }

    std::shared_ptr<CallbackReceiver<void>> receiver() { return m_callback_receiver; }

    void wait(std::chrono::milliseconds timeout)
    {
        if (m_future.wait_for(timeout) != std::future_status::ready)
            throw std::runtime_error("Operation timeouted");
    }


private:
    std::shared_ptr<CallbackReceiver<void>> m_callback_receiver;
    std::promise<void> m_promise;
    std::future<void> m_future{m_promise.get_future()};
};


class IOServiceRunner
{
public:
    static std::shared_ptr<IOServiceRunner> create();
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

protected:
    IOServiceRunner();

private:

    boost::asio::io_service m_io_service;
    std::shared_ptr<boost::asio::io_service::work> m_work;
    std::thread m_service_thread;
};

#endif // ASIO_TASK_HPP
