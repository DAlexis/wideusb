#include "communication/json/identificator.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"

#include "json/helpers-json.hpp"

void IdentificatorJSON::put_massage_id(PBuffer buf, MessageId id)
{

}

std::optional<MessageId> IdentificatorJSON::get_message_id(PBuffer buf, size_t& pos)
{
    return std::nullopt;
}
