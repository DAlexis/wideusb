#ifndef MODULE_HPP_INCLUDED
#define MODULE_HPP_INCLUDED

#include "tools/json-objects.hpp"
#include "rapidjson-config.h"
#include "rapidjson/document.h"

#include <string>

class IHostCommunicator;

enum class AssertLevel {
    Debug = 0,
    Error = 1
};

class IModule
{
public:
    virtual const char* name() = 0;
    virtual void add_filter(ObjectFilter& target_filter) = 0;
    virtual void connect_to_communicator(IHostCommunicator* communicator) = 0;

    virtual ~IModule() = default;
};

class Module : public IModule
{
public:
    Module(const char* name);
    void add_filter(ObjectFilter& target_filter) override;
    void connect_to_communicator(IHostCommunicator* communicator) override;

    void assert_text(const char* text, AssertLevel level = AssertLevel::Debug, bool copy_text = false);
    const char* name() override;

protected:
    void add_module_field(rapidjson::Document& doc);

    ObjectFilter m_action_filter{"action"};
    PropertiesCollection m_properties_collection;

    IHostCommunicator* m_communicator = nullptr;
    const char* m_name;
};

#endif // MODULE_HPP_INCLUDED
