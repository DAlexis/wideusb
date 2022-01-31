#include "wideusb-pc/asio-utils.hpp"

#include "gtest/gtest.h"
#include <thread>

using namespace std::chrono_literals;

TEST(Synchronizer, WaitingForAsyncOp)
{
    {
        Waiter<int> waiter;
        CallbackEntry<int> entry_point = waiter.receiver();
        std::thread t([&entry_point]() mutable { std::this_thread::sleep_for(10ms); entry_point.call(32); });

        ASSERT_NO_THROW(waiter.wait(1s));
        t.join();
    }
    {
        Waiter<void> waiter;
        CallbackEntry<void> entry_point = waiter.receiver();
        std::thread t([&entry_point]() mutable { std::this_thread::sleep_for(100ms); entry_point.call(); });

        ASSERT_ANY_THROW(waiter.wait(1ms));
        t.join();
    }
}

