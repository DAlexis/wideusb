#ifndef NETSRVRUNNER_HPP
#define NETSRVRUNNER_HPP

#include <functional>
#include <chrono>

class INetServiceRunner
{
public:
    virtual ~INetServiceRunner() = default;
    virtual void set_callback(std::function<void(void)> serve_sockets_callback) = 0;
    virtual void post_serve_sockets(std::chrono::milliseconds delay_from_now) = 0;
    virtual bool is_posted() = 0;
    virtual void cancel() = 0;
};


#endif // NETSRVRUNNER_HPP
