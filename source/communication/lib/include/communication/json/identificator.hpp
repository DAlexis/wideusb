#ifndef JSON_IDENTIFICATOR_HPP
#define JSON_IDENTIFICATOR_HPP

#include "communication/identification.hpp"

class IdentificatorJSON : public IMessageIdentificator
{
public:
    bool put_message_id(SerialWriteAccessor& buf, const MessageHeader& header) override;

    std::optional<MessageHeader> get_message_id(SerialReadAccessor& accessor) override;
};

#endif // JSON_IDENTIFICATOR_HPP
