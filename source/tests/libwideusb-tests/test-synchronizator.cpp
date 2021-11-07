#include "wideusb/asio-utils.hpp"

#include "gtest/gtest.h"

using namespace std::chrono_literals;

/*
TEST(Synchronizer, AsioTest)
{

}*/


TEST(Synchronizer, WaitingForAsyncOp)
{
    boost::asio::io_service io_service;
    AsioServiceRunner runner(io_service);
    runner.run_thread();
    Waiter<void> waiter;
    DeferredTask::run(io_service, 1000, waiter.get_waiter_callback());
    waiter.wait();
}

