#ifndef SOCKETQUEUEMUTEX_HPP
#define SOCKETQUEUEMUTEX_HPP

#include "wideusb/communication/socket.hpp"
#include <list>
#include <mutex>

template <typename T>
class MutexQueue : public IQueue<T>
{
public:
    MutexQueue(size_t size) : m_size(size)
    {
    }

    void push(const T& data) override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push_back(data);
    }

    void push_front(const T& data) override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push_front(data);
    }

    T& front() override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.front();
    }

    void pop() override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_queue.empty())
            m_queue.pop_front();
    }

    size_t size() override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    size_t capacity() override { return m_size; }

private:
    size_t m_size;
    std::mutex m_mutex;
    std::list<T> m_queue;
};


class MutexQueueFactory : public IQueueFactory
{
public:
    IQueue<ISocketSystemSide::OutgoingMessage>::Ptr produce_outgoing_queue(size_t size) override;
    IQueue<ISocketUserSide::IncomingMessage>::Ptr produce_incomming_queue(size_t size) override;
};


#endif // SOCKETQUEUEMUTEX_HPP
