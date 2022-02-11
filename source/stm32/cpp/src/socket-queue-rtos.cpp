#include "socket-queue-rtos.hpp"

IQueue<ISocketSystemSide::OutgoingMessage>::Ptr QueueFactory::produce_outgoing_queue(size_t size)
{
    return std::make_shared<MutexQueue<ISocketSystemSide::OutgoingMessage>>(size);
}

IQueue<ISocketUserSide::IncomingMessage>::Ptr QueueFactory::produce_incomming_queue(size_t size)
{
    return std::make_shared<MutexQueue<ISocketUserSide::IncomingMessage>>(size);
}
