#include "communication/json/messages-core.hpp"
#include "communication/json/rapidjson-config.h"
#include "communication/json/helpers-json.hpp"

#include "rapidjson/document.h"


using namespace rapidjson;

bool StatusRequestSerializer::serialize(SerialWriteAccessor& accessor, const Message& msg)
{
    const StatusRequest* request = message_cast<StatusRequest>(&msg);
    if (request == nullptr)
        return false;
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    d.AddMember("do_blink", Value(request->do_blink), alloc);

    return accessor.put(BufferAccessor(buffer_from_document(d)));
}

std::shared_ptr<Message> StatusRequestSerializer::deserialize(SerialReadAccessor& accessor)
{
    Document d;
    PBuffer buf = Buffer::create(accessor);

    d.Parse((const char*) buf->data(), buf->size());

    if (!d.IsObject() || !d.HasMember("do_blink") || !d["do_blink"].IsBool())
        return nullptr;

    std::shared_ptr<StatusRequest> parsed_msg = std::make_shared<StatusRequest>();
    parsed_msg->do_blink = d["do_blink"].GetBool();
    return parsed_msg;
}
