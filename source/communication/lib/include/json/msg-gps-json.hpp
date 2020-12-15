#ifndef GPSMSGJSON_HPP
#define GPSMSGJSON_HPP

#include "messages/gps-messages.hpp"
#include "json/json-base.hpp"

//////////////////////////
// GPSPoint
template<>
class JSONSerializer<GPSPoint> : public IMessageSerializer
{
public:
    PBuffer serialize(const Message* msg) const override;
};

template<>
class JSONDeserializer<GPSPoint> : public MessageDeserializer<GPSPoint>
{
public:
    JSONDeserializer(MessageDeserializer::Callback callback);
protected:
    bool parse_impl(GPSPoint& target, const PBuffer buffer) override;
};

#endif // GPSMSGJSON_HPP
