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
    void connect_to_comminucator(IHostCommunicator* communicator) override;

    void report_debug(const std::string& text);
    void report_error(const std::string& text);

protected:
    void add_module_field(rapidjson::Document& doc);


    IHostCommunicator* m_communicator;
};

#endif // MODULE_HPP_INCLUDED
