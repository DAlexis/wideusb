#ifndef MODULE_HPP_INCLUDED
#define MODULE_HPP_INCLUDED

#include "tools/property-map.hpp"
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
    /// @todo Give unique_ptr to message to allow module having ist own queue or process message laiter
    virtual void receive_message(const rapidjson::Document& doc) = 0;
    virtual void connect_to_comminucator(IHostCommunicator* communicator) = 0;

    virtual ~IModule() = default;
};

class Module : public IModule
{
public:
    Module(const char* name);
    void connect_to_comminucator(IHostCommunicator* communicator) override;
    void assert_text(const char* text, AssertLevel level = AssertLevel::Debug, bool copy_text = false);
    const char* name() override;

protected:
    void add_module_field(rapidjson::Document& doc);

    ActionsCollection m_actions_collection;
    PropertiesCollection m_properties_collection;

    IHostCommunicator* m_communicator = nullptr;
    const char* m_name;
};

#endif // MODULE_HPP_INCLUDED
