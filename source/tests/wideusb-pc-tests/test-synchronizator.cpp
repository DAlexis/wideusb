#include "wideusb-pc/asio-utils.hpp"

#include "gtest/gtest.h"

using namespace std::chrono_literals;

TEST(Synchronizer, WaitingForAsyncOp)
{
    auto service_runner = IOServiceRunner::create();

    Waiter<void> waiter;
    DeferredTask::run(service_runner->io_service(), 200ms, waiter.get_waiter_callback());
    waiter.wait();
}

