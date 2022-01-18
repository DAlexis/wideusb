#include "wideusb-pc/socket-queue-mutex.hpp"


IQueue<ISocketSystemSide::OutgoingMessage>::Ptr MutexQueueFactory::produce_outgoing_queue(size_t size)
{
    return std::make_shared<MutexQueue<ISocketSystemSide::OutgoingMessage>>(size);
}

IQueue<ISocketUserSide::IncomingMessage>::Ptr MutexQueueFactory::produce_incomming_queue(size_t size)
{
    return std::make_shared<MutexQueue<ISocketUserSide::IncomingMessage>>(size);
}
