#include "wideusb-pc/asio-net-srv-runner.hpp"

using namespace std::literals::chrono_literals;

std::unique_ptr<INetServiceRunner> NetServiceRunnerAsio::create(std::shared_ptr<IOServiceRunner> runner)
{
    return std::unique_ptr<INetServiceRunner>(new NetServiceRunnerAsio(runner));
}

NetServiceRunnerAsio::NetServiceRunnerAsio(std::shared_ptr<IOServiceRunner> runner) :
    m_runner(runner), m_timer(m_runner->io_service())
{
}

void NetServiceRunnerAsio::set_callback(std::function<void(void)> serve_sockets_callback)
{
    m_serve_sockets_callback = serve_sockets_callback;
}

void NetServiceRunnerAsio::post_serve_sockets(std::chrono::milliseconds delay_from_now)
{
    m_is_canceled = false;
    m_is_posted = true;
    if (delay_from_now == 0ms)
    {
        boost::asio::post(m_runner->io_service(), [this]() { callback_wrapper(boost::system::error_code()); });
    } else {
        m_timer.expires_from_now(boost::posix_time::milliseconds(delay_from_now.count()));
        m_timer.async_wait([this](const boost::system::error_code& ec){ callback_wrapper(ec); });
    }
}

bool NetServiceRunnerAsio::is_posted()
{
    return m_is_posted;
}

void NetServiceRunnerAsio::cancel()
{
    if (m_is_posted)
    {
        m_is_canceled = true;
        m_timer.cancel();
    }
}

void NetServiceRunnerAsio::callback_wrapper(const boost::system::error_code& ec)
{
    if (m_is_canceled || ec == boost::asio::error::operation_aborted)
    {
        m_is_canceled = false;
        m_is_posted = false;
        return;
    }
    m_serve_sockets_callback();
}
