#ifndef IDENTIFICATION_HPP
#define IDENTIFICATION_HPP

#include "communication/message.hpp"
#include "buffer.hpp"

#include <optional>

constexpr static MessageId MSG_ID_ACK = 1;

using MessageKey = uint32_t;

struct MessageHeader
{
    MessageHeader(MessageId id, uint32_t key) :
        id(id), key(key)
    { }
    /**
     * @brief id   Message type idenfitier
     */
    MessageId id = 0;

    /**
     * @brief key  Concrete message identifier for acknoledgement purpose
     */
    MessageKey key = 0;
};

class IMessageIdentificator
{
public:
    virtual bool put_message_id(SerialWriteAccessor& buf, const MessageHeader& header) = 0;
    /**
     * @brief Parse message id identificator from Buffer and move reading pointer
     * @param buf   source buffer
     * @param pos   index in buffer that will be moved
     * @return Message id if parsing is OK
     */
    virtual std::optional<MessageHeader> get_message_id(SerialReadAccessor& accessor) = 0;
};

#endif // IDENTIFICATION_HPP
