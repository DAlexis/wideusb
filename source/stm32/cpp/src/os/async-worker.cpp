#include "os/async-worker.hpp"

AsyncWorker::AsyncWorker(size_t stack_size, size_t queue_size, const char* thread_name) :
    m_thread([this](){ thread_body(); }, thread_name, stack_size),
    m_queue(queue_size)
{
    m_thread.run();
}

bool AsyncWorker::post(AsyncWorker::Task task)
{
    if (!task)
        return false;
    return m_queue.push_back(task);
}

void AsyncWorker::thread_body()
{
    for (;;)
    {
        auto task = m_queue.pop_front();
        if (task.has_value())
            (*task)();
    }
}

