#ifndef MODULEBASE_HPP
#define MODULEBASE_HPP

#include <cstdint>

using ModuleID = uint32_t;

class IModule
{
public:
    virtual ~IModule() = default;
};

class ModuleBase : IModule
{
public:
protected:

};

#endif // MODULEBASE_HPP
