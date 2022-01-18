#ifndef SOCKETQUEUERTOS_HPP
#define SOCKETQUEUERTOS_HPP

#include "wideusb/communication/socket.hpp"
#include "os/cpp-freertos.hpp"

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

    T front() override
    {
        return *m_queue.front();
    }

    void pop() override
    {
        m_queue.pop_front();
    }

    size_t size() override { return m_queue.size(); }
    size_t capacity() override { return m_queue.capacity(); }

private:
    os::QueuePointerBased<T> m_queue;
};

class QueueFactory : public IQueueFactory
{
public:
    IQueue<ISocketSystemSide::OutgoingMessage>::Ptr produce_outgoing_queue(size_t size) override;
    IQueue<ISocketUserSide::IncomingMessage>::Ptr produce_incomming_queue(size_t size) override;
};

#endif // SOCKETQUEUERTOS_HPP
