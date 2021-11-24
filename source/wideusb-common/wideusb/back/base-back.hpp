#ifndef MODULEBASE_HPP
#define MODULEBASE_HPP

#include <cstdint>

class IModule
{
public:
    virtual void tick() = 0;
    virtual ~IModule() = default;
};

#endif // MODULEBASE_HPP
