#ifndef JSON_MESSAGES_CORE_HPP
#define JSON_MESSAGES_CORE_HPP

#include "communication/serialization.hpp"

#include "messages/core-messages.hpp"

class StatusRequestSerializer : public MessageSerializer
{
public:
    StatusRequestSerializer() : MessageSerializer(StatusRequest::id) { }
    bool serialize(SerialWriteAccessor& accessor, const Message& msg) override;
    std::shared_ptr<Message> deserialize(SerialReadAccessor& accessor) override;
};


#endif // JSON_MESSAGES_CORE_HPP
