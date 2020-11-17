#include "serialization-json/core-msg-serialization.hpp"

StatusRequestSerializer::StatusRequestSerializer(const StatusRequest& request) :
    m_request(request)
{
}

PBuffer StatusRequestSerializer::serialize()
{
    PBuffer result;
    return result;
}

bool StatusRequestDeserializer::parse_impl(StatusRequest& target, RingBuffer& buffer)
{
    return false;
}
