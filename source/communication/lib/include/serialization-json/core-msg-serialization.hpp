#ifndef COREMSGSERIALIZATION_HPP
#define COREMSGSERIALIZATION_HPP

#include "messages/core-messages.hpp"
#include "serialization-json/serialization-json-base.hpp"

//////////////////////////
// Status request
template<>
class JSONSerializer<StatusRequest> : public IMessageSerializer
{
public:
    JSONSerializer(const StatusRequest& request);
    PBuffer serialize() const override;
private:
    const StatusRequest& m_request;
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
    JSONSerializer(const StatusResponse& response);
    PBuffer serialize() const override;
private:
    const StatusResponse& m_response;
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
