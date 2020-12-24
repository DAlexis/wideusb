#ifndef IDENTIFICATOR_HPP
#define IDENTIFICATOR_HPP

#include "communication/identification.hpp"

class IdentificatorJSON : public IMessageIdentificator
{
public:
    void put_message_id(PBuffer buf, MessageId id) override;

    std::optional<MessageId> get_message_id(SerialReadAccessor& accessor) override;
};

#endif // IDENTIFICATOR_HPP
