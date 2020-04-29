#include "os/cpp-freertos.hpp"

using namespace os;

class Test
{
public:
    Test()
    {
        int i = 0;
        i += 1;
        TaskBase tb;
        Queue<int> q(10);

    }
};

Test t;
