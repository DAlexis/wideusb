#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#include "communication/receiver.hpp"
#include "communication/sender.hpp"

class Communicator
{
public:
    Communicator(std::shared_ptr<IStreamificator> streamificator, std::shared_ptr<IDestreamificator> destreamificator, std::shared_ptr<IMessageIdentificator> identificator);

private:
    MessageListener m_listener;
    MessageTransmitter m_transmitter;

};

#endif // COMMUNICATOR_HPP
