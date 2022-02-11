#ifndef SOCKETQUEUERTOS_HPP
#define SOCKETQUEUERTOS_HPP

#include "wideusb/communication/socket.hpp"
#include "os/cpp-freertos.hpp"

#include <list>
#include <mutex>
/*
template <typename T>
class RTOSQueue : public IQueue<T>
{
public:
    RTOSQueue(size_t size) :
        m_queue(size)
    {
    }

    void push(const T& data) override
    {
        m_queue.push_back(data);
    }

    void push_front(const T& data) override
    {
        m_queue.push_front(data);
    }

    T& front() override
    {
        return m_queue.front_ref();
    }

    void pop() override
    {
        m_queue.pop_front();
    }

    size_t size() override { return m_queue.size(); }
    size_t capacity() override { return m_queue.capacity(); }

private:
    os::QueuePointerBased<T> m_queue;
};*/

template <typename T>
class MutexQueue : public IQueue<T>
{
public:
    MutexQueue(size_t size) : m_size(size)
    {
    }

    void push(const T& data) override
    {
        std::unique_lock<os::Mutex> lck(m_mutex);
        m_queue.push_back(data);

    }

    void push_front(const T& data) override
    {
        std::unique_lock<os::Mutex> lck(m_mutex);
        m_queue.push_front(data);
    }

    T& front() override
    {
        std::unique_lock<os::Mutex> lck(m_mutex);
        return m_queue.front();
    }

    void pop() override
    {
        std::unique_lock<os::Mutex> lck(m_mutex);
        if (!m_queue.empty())
            m_queue.pop_front();
    }

    size_t size() override
    {
        std::unique_lock<os::Mutex> lck(m_mutex);
        return m_queue.size();
    }

    size_t capacity() override { return m_size; }

private:
    size_t m_size;
    os::Mutex m_mutex;
    std::list<T> m_queue;
};

class QueueFactory : public IQueueFactory
{
public:
    IQueue<ISocketSystemSide::OutgoingMessage>::Ptr produce_outgoing_queue(size_t size) override;
    IQueue<ISocketUserSide::IncomingMessage>::Ptr produce_incomming_queue(size_t size) override;
};

#endif // SOCKETQUEUERTOS_HPP
