#include "modules/core.hpp"
#include "host-communication-interface.hpp"

const char* CoreModule::name()
{
    return "core";
}

void CoreModule::receive_message(const rapidjson::Document& doc)
{

}
