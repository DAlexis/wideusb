#ifndef STREAMIFICATION_HPP
#define STREAMIFICATION_HPP

#include "buffer.hpp"

#include <optional>

class IStreamificator
{
public:
    virtual bool pack(RingBuffer& ring_buffer, const PBuffer buffer) = 0;

    virtual ~IStreamificator() = default;
};

class IDestreamificator
{
public:
    virtual std::optional<PBuffer> unpack(SerialReadAccessor& ring_buffer) = 0;
    virtual void reset() = 0;

    virtual ~IDestreamificator() = default;
};

#endif // STREAMIFICATION_HPP
