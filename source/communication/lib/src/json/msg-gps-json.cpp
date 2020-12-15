#include "json/msg-gps-json.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"

#include "json/helpers-json.hpp"

using namespace rapidjson;

PBuffer JSONSerializer<GPSPoint>::serialize(const Message* msg) const
{
    const GPSPoint* gps_point = static_cast<const GPSPoint*>(msg);
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    d.AddMember("has_position", Value(gps_point->has_position), alloc);
    if (gps_point->has_position)
    {
        d.AddMember("latitude", Value(gps_point->latitude), alloc);
        d.AddMember("longitude", Value(gps_point->longitude), alloc);
        d.AddMember("altitude", Value(gps_point->altitude), alloc);
    }
    d.AddMember("sec_epoch", Value(gps_point->sec_epoch), alloc);
    d.AddMember("year", Value(gps_point->year), alloc);
    d.AddMember("month", Value(gps_point->month), alloc);
    d.AddMember("day", Value(gps_point->day), alloc);
    d.AddMember("hour", Value(gps_point->hour), alloc);
    d.AddMember("min", Value(gps_point->min), alloc);
    d.AddMember("sec", Value(gps_point->sec), alloc);
    return buffer_from_document(d);
}

JSONDeserializer<GPSPoint>::JSONDeserializer(MessageDeserializer::Callback callback) :
    MessageDeserializer<GPSPoint>(callback)
{
}

bool JSONDeserializer<GPSPoint>::parse_impl(GPSPoint& target, const PBuffer buffer)
{
    return false;
}
