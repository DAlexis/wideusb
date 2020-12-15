#ifndef COREMSGSERIALIZATION_HPP
#define COREMSGSERIALIZATION_HPP

#include "messages/core-messages.hpp"
#include "json/json-base.hpp"

//////////////////////////
// Status request
template<>
class JSONSerializer<StatusRequest> : public IMessageSerializer
{
public:
    PBuffer serialize(const Message* msg) const override;
};

template<>
class JSONDeserializer<StatusRequest> : public MessageDeserializer<StatusRequest>
{
public:
    JSONDeserializer(MessageDeserializer::Callback callback);
protected:
    bool parse_impl(StatusRequest& target, const PBuffer buffer) override;
};

//////////////////////////
// Status response
template<>
class JSONSerializer<StatusResponse> : public IMessageSerializer
{
public:
    PBuffer serialize(const Message* msg) const override;
};

template<>
class JSONDeserializer<StatusResponse> : public MessageDeserializer<StatusResponse>
{
public:
    JSONDeserializer(MessageDeserializer::Callback callback);
protected:
    bool parse_impl(StatusResponse& target, const PBuffer buffer) override;

};



#endif // COREMSGSERIALIZATION_HPP
