#include "net-srv-runner.hpp"

std::unique_ptr<INetServiceRunner> NetSrvRunner::create()
{
    return std::unique_ptr<INetServiceRunner>(new NetSrvRunner);
}

NetSrvRunner::NetSrvRunner()
{
    m_serve_socket_thread.run();
}

void NetSrvRunner::set_callback(std::function<void(void)> serve_sockets_callback)
{
    m_serve_sockets_callback = serve_sockets_callback;
}

void NetSrvRunner::post_serve_sockets(std::chrono::milliseconds delay_from_now)
{
    m_time_point_to_run = std::chrono::steady_clock::now() + delay_from_now;
    m_serve_socket_thread.notify_give();
}

bool NetSrvRunner::is_posted()
{
    return true;
}

void NetSrvRunner::cancel()
{
}

void NetSrvRunner::thread_body()
{
    for (;;)
    {
        os::Thread::notify_take(true, 1000);

        while (std::chrono::steady_clock::now() < m_time_point_to_run)
            os::delay(1ms);

        if (m_serve_sockets_callback)
            m_serve_sockets_callback();
    }
}
