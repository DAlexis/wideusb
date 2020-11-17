#ifndef COREMSGSERIALIZATION_HPP
#define COREMSGSERIALIZATION_HPP

#include "messages/core-messages.hpp"

#include "message-send.hpp"
#include "message-receive.hpp"

class StatusRequestSerializer : public IMessageSerializer
{
public:
    StatusRequestSerializer(const StatusRequest& request);

    PBuffer serialize() override;

private:
    const StatusRequest& m_request;
};

class StatusRequestDeserializer : public MessageDeserializer<StatusRequest>
{
public:
    StatusRequestDeserializer(MessageDeserializer::Callback callback) :
        MessageDeserializer(callback)
    { }

protected:
    bool parse_impl(StatusRequest& target, RingBuffer& buffer) override;

};

#endif // COREMSGSERIALIZATION_HPP
