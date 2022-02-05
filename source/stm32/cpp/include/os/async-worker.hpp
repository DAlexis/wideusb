#ifndef ASYNCWORKER_HPP
#define ASYNCWORKER_HPP

#include "os/cpp-freertos.hpp"
#include <functional>

class AsyncWorker
{
public:
    using Task = std::function<void(void)>;

    AsyncWorker(size_t stack_size, size_t queue_size, const char* thread_name = "");
    bool post(Task task);

private:
    void thread_body();

    os::Thread m_thread;
    os::QueuePointerBased<Task> m_queue;
};

#endif // ASYNCWORKER_HPP
