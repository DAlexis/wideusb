#include "wideusb/utils/utilities.hpp"

#include "gtest/gtest.h"

TEST(CallbackReceiver, Operating)
{
    int val = 0;
    {
        auto cb1 = CallbackReceiver<int>::create([&val](int a){ val = a; });

        CallbackEntry ep(cb1);
        ep.call(123);
        ASSERT_EQ(val, 123);

        cb1.reset(); // Callback receiver eliminated

        ep.call(321);
        ASSERT_EQ(val, 123);
    }
    {
        val = 0;
        auto cb2 = CallbackReceiver<void>::create([&val](){ val = 456; });

        CallbackEntry ep(cb2);
        ep.call();
        ASSERT_EQ(val, 456);
        val = 0;

        cb2.reset(); // Callback receiver eliminated

        ep.call();
        ASSERT_EQ(val, 0);
    }
}
