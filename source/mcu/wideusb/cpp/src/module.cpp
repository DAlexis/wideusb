#include "module.hpp"
#include "host-communication.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"

using namespace rapidjson;

void Module::connect_to_comminucator(IHostCommunicator* communicator)
{
    m_communicator = communicator;
}

void Module::add_module_field(rapidjson::Document& doc)
{
    auto & alloc = doc.GetAllocator();

    Value module(kStringType);
    module.SetString(StringRef(name()));
    doc.AddMember("module", module, alloc);
}

void Module::report_debug(const std::string& text)
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    add_module_field(d);

    Value action("debug");
    d.AddMember("action", action, alloc);

    Value error_text(kStringType);
    error_text.SetString(StringRef(text.c_str()));
    d.AddMember("text", error_text, alloc);

    m_communicator->send_data(d);
}

void Module::report_error(const std::string& text)
{
    Document d;
    d.SetObject();
    auto & alloc = d.GetAllocator();

    add_module_field(d);

    Value action("error");
    d.AddMember("action", action, alloc);

    Value error_text(kStringType);
    error_text.SetString(StringRef(text.c_str()));
    d.AddMember("text", error_text, alloc);

    m_communicator->send_data(d);
}
