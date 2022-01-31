#ifndef STM32_NETSRVRUNNER_HPP
#define STM32_NETSRVRUNNER_HPP

#include "wideusb/communication/net-srv-runner.hpp"
#include "os/cpp-freertos.hpp"

class NetSrvRunner : public INetServiceRunner
{
public:
    static std::unique_ptr<INetServiceRunner> create();

    NetSrvRunner();
    void set_callback(std::function<void(void)> serve_sockets_callback) override;
    void post_serve_sockets(std::chrono::milliseconds delay_from_now) override;
    bool is_posted() override;
    void cancel() override;

private:
    void thread_body();

    os::Thread m_serve_socket_thread{[this](){ thread_body(); }, "net_srv_runner", 1024};
    std::chrono::steady_clock::time_point m_time_point_to_run;
    bool m_is_posted = false;
    bool m_run_just_now = false;

    std::function<void(void)> m_serve_sockets_callback;
};

#endif // STM32_NETSRVRUNNER_HPP
