#ifndef HOST_COMMUNICATION_INTERFACE_INCLUDED
#define HOST_COMMUNICATION_INTERFACE_INCLUDED

#include "module.hpp"
#include "rapidjson/document.h"

#include <memory>

class IHostCommunicator
{
public:
    virtual void send_data(std::unique_ptr<rapidjson::Document> doc) = 0;
    virtual void add_module(IModule* module) = 0;

    virtual ~IHostCommunicator() = default;
};

#endif // HOST_COMMUNICATION_INTERFACE_INCLUDED
