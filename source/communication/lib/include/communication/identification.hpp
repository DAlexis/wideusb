#ifndef IDENTIFICATION_HPP
#define IDENTIFICATION_HPP

#include "communication/message.hpp"
#include "buffer.hpp"

#include <optional>

class IMessageIdentificator
{
public:
    virtual void put_message_id(PBuffer buf, MessageId id) = 0;
    /**
     * @brief Parse message id identificator from Buffer and move reading pointer
     * @param buf   source buffer
     * @param pos   index in buffer that will be moved
     * @return Message id if parsing is OK
     */
    virtual std::optional<MessageId> get_message_id(PBuffer buf, size_t& pos) = 0;
};

#endif // IDENTIFICATION_HPP
