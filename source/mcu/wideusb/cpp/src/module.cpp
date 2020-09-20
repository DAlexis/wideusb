#include "module.hpp"
#include "host-communication.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"

using namespace rapidjson;

Module::Module(const char* name) :
    m_name(name)
{
    m_properties_collection.add_filter(m_action_filter);
}

void Module::add_filter(ObjectFilter& target_filter)
{
    m_action_filter.add_action(name(), target_filter);
}

void Module::connect_to_communicator(IHostCommunicator* communicator)
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

void Module::assert_text(const char* text, AssertLevel level, bool copy_text)
{
    std::unique_ptr<Document> d(new Document());

    d->SetObject();
    auto & alloc = d->GetAllocator();

    add_module_field(*d);

    Value action(kStringType);
    switch(level)
    {
    case AssertLevel::Debug:
        action.SetString(StringRef("debug"));
        break;
    default:
    case AssertLevel::Error:
        action.SetString(StringRef("error"));
        break;

    }
    d->AddMember("action", action, alloc);

    Value error_text(kStringType);

    if (copy_text)
        error_text.SetString(text, strlen(text), alloc);
    else
        error_text.SetString(StringRef(text));

    d->AddMember("text", error_text, alloc);

    m_communicator->send_data(std::move(d));
}

const char* Module::name()
{
    return m_name;
}
