#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <cstring>

template<typename T>
void zerify(T& target)
{
    memset(&target, 0, sizeof(T));
}

template<typename FunctionType>
class Callback
{
public:
    void set(FunctionType func);

private:
    FunctionType m_func = nullptr;
};

#endif // UTILITIES_HPP_INCLUDED
