#include "communication/communicator.hpp"

Communicator::Communicator(
        std::shared_ptr<IStreamificator> streamificator,
        std::shared_ptr<IDestreamificator> destreamificator,
        std::shared_ptr<IMessageIdentificator> identificator) :
    m_listener(destreamificator, identificator),
    m_transmitter(streamificator, identificator)
{
}
