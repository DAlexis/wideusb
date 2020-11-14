#include "ring-buffer.h"

#include "gtest/gtest.h"

#include <iostream>

using namespace std;

TEST(RingBufferTest, BasicIO)
{
    RingBuffer buffer;
    std::vector<uint8_t> contents(10);
    ASSERT_NO_THROW(ring_buffer_init(&buffer, contents.data(), contents.size()));

    ASSERT_EQ(ring_buffer_free_space(&buffer), contents.size());
    ASSERT_EQ(ring_buffer_data_size(&buffer), 0);
}
