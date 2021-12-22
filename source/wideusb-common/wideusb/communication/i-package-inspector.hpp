#ifndef IPACKAGEINSPECTOR_HPP
#define IPACKAGEINSPECTOR_HPP

#include "wideusb/buffer.hpp"
#include <string>

class IPackageInspector
{
public:
    virtual ~IPackageInspector() = default;
    virtual void inspect_package(const PBuffer data, const std::string& context_msg = "") = 0;
};

#endif // IPACKAGEINSPECTOR_HPP
