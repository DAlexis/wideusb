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
