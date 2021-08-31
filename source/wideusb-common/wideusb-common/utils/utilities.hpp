#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <cstring>

template<typename T>
void zerify(T& target)
{
    memset(&target, 0, sizeof(T));
}

#endif // UTILITIES_HPP_INCLUDED
