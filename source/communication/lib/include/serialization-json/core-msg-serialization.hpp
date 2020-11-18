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

class StatusResponseSerializer : public IMessageSerializer
{
public:
    StatusResponseSerializer(const StatusResponse& response);

    PBuffer serialize() override;

private:
    const StatusResponse& m_response;
};

class StatusResponseDeserializer : public MessageDeserializer<StatusResponse>
{
public:
    StatusResponseDeserializer(MessageDeserializer::Callback callback) :
        MessageDeserializer(callback)
    { }

protected:
    bool parse_impl(StatusResponse& target, RingBuffer& buffer) override;

};



#endif // COREMSGSERIALIZATION_HPP
