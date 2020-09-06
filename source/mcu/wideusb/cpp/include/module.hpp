#ifndef MODULE_HPP_INCLUDED
#define MODULE_HPP_INCLUDED

#include "rapidjson-config.h"
#include "rapidjson/document.h"

#include <string>

class IHostCommunicator;

class IModule
{
public:
    virtual const char* name() = 0;
    virtual void receive_message(const rapidjson::Document& doc) = 0;
    virtual void connect_to_comminucator(IHostCommunicator* communicator) = 0;

    virtual ~IModule() = default;
};

class Module : public IModule
{
public:
    enum class AssertLevel {
        Debug = 0,
        Error = 1
    };

    void connect_to_comminucator(IHostCommunicator* communicator) override;

    void assert_text(const char* text, AssertLevel level = AssertLevel::Debug, bool copy_text = false);

protected:
    void add_module_field(rapidjson::Document& doc);


    IHostCommunicator* m_communicator;
};

#endif // MODULE_HPP_INCLUDED
