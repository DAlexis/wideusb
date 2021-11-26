#include "wideusb/buffer.hpp"

#include "gtest/gtest.h"

#include <iostream>

using namespace std;

class RingBufferTest : public ::testing::Test
{
protected:
    void SetUp() override {
        buffer.reset(new RingBuffer(buffer_size));
    }

    const size_t buffer_size = 10;

    std::unique_ptr<RingBuffer> buffer;

    uint8_t test_data[4] = {12, 23, 34, 45};
    uint8_t receiver[10];
};

TEST(RingBufferTestBasic, Instantiation)
{
    RingBuffer buffer(10);

    ASSERT_EQ(buffer.free_space(), 10);
    ASSERT_EQ(buffer.size(), 0);
}

TEST_F(RingBufferTest, SimplePutGet)
{
    ASSERT_NO_THROW(buffer->put(test_data, sizeof(test_data)));

    ASSERT_EQ(buffer->size(), 4);
    ASSERT_EQ(buffer->free_space(), buffer_size - 4);

    size_t size = buffer->size();
    ASSERT_NO_THROW(buffer->extract(receiver, size));

    ASSERT_EQ(0, memcmp(test_data, receiver, size));

    ASSERT_EQ(buffer->size(), 0);
    ASSERT_EQ(buffer->free_space(), buffer_size);
}

TEST_F(RingBufferTest, CyclicPutGet)
{
    for (int i = 0; i < 100; i++)
    {
        ASSERT_NO_THROW(buffer->put(test_data, sizeof(test_data)));

        ASSERT_EQ(buffer->size(), sizeof(test_data));

        ASSERT_NO_THROW(buffer->put(test_data, sizeof(test_data)));

        ASSERT_EQ(buffer->size(), 2 * sizeof(test_data));

        size_t size = sizeof(test_data);
        ASSERT_NO_THROW(buffer->extract(receiver, size));

        ASSERT_EQ(0, memcmp(test_data, receiver, size));
        ASSERT_EQ(buffer->size(), sizeof(test_data));

        ASSERT_NO_THROW(buffer->extract(receiver, size));

        ASSERT_EQ(0, memcmp(test_data, receiver, size));
        ASSERT_EQ(buffer->size(), 0);
    }
}

TEST_F(RingBufferTest, AtSkip)
{
    buffer->put(test_data, sizeof(test_data)); // (0,4)
    buffer->extract(receiver, sizeof(test_data));

    buffer->put(test_data, sizeof(test_data));  // (4,8)
    buffer->put(test_data, sizeof(test_data)); // (8,2)

    ASSERT_EQ((*buffer)[1], 23);
    ASSERT_NO_THROW(buffer->skip(2));
    ASSERT_EQ((*buffer)[1], 45);
}

TEST_F(RingBufferTest, PutFromBuffer)
{
    auto linear_buf = Buffer::create();
    float test_val = 3.1415;
    linear_buf->put(&test_val, sizeof(test_val));
    BufferAccessor buff_acc(linear_buf);
    *buffer << buff_acc;

    ASSERT_EQ(buffer->size(), sizeof(test_val));

    auto lib_buf_2 = Buffer::create();
    *lib_buf_2 << *buffer;

    ASSERT_EQ(lib_buf_2->size(), sizeof(test_val));

    float from_buffer = *reinterpret_cast<float*>(lib_buf_2->data());

    ASSERT_EQ(from_buffer, test_val);
}

TEST_F(RingBufferTest, GetContiniousBlock)
{
    buffer->put(test_data, sizeof(test_data));
    MemBlock block = buffer->get_continious_block(100);
    ASSERT_EQ(block.size, sizeof(test_data));
    ASSERT_EQ(buffer->free_space(), 10-4);

    buffer->put(test_data, sizeof(test_data));
    block = buffer->get_continious_block(100);
    ASSERT_EQ(block.size, 2*sizeof(test_data));
    // 8 of 10 (11)
    buffer->skip(5); // read -> 5
    buffer->put(test_data, sizeof(test_data)); // w -> 1?

    block = buffer->get_continious_block(100);
    ASSERT_EQ(block.size, 6);
    buffer->skip(block.size);
    block = buffer->get_continious_block(100);
    ASSERT_EQ(block.size, 1);
}

