#ifndef ASIONETSRVRUNNER_HPP
#define ASIONETSRVRUNNER_HPP

#include "wideusb/communication/net-srv-runner.hpp"
#include "wideusb-pc/asio-utils.hpp"

class NetServiceRunnerAsio : public INetServiceRunner
{
public:
    static std::unique_ptr<INetServiceRunner> create(std::shared_ptr<IOServiceRunner> runner);

    void set_callback(std::function<void(void)> serve_sockets_callback) override;
    void post_serve_sockets(std::chrono::milliseconds delay_from_now) override;
    bool is_posted() override;
    void cancel() override;

private:
    NetServiceRunnerAsio(std::shared_ptr<IOServiceRunner> runner);

    void callback_wrapper(const boost::system::error_code& ec);

    std::shared_ptr<IOServiceRunner> m_runner;

    boost::asio::deadline_timer m_timer;
    std::function<void(void)> m_serve_sockets_callback;
    bool m_is_posted = false;
    bool m_is_canceled = false;
};

#endif // ASIONETSRVRUNNER_HPP
