#include "ring-buffer.h"
#include "buffer.hpp"

#include "gtest/gtest.h"

#include <iostream>

using namespace std;

class RingBufferTest : public ::testing::Test
{
protected:
    void SetUp() override {
        contents.resize(10);
        ring_buffer_init(&buffer, contents.data(), contents.size());
    }

    RingBuffer buffer;
    std::vector<uint8_t> contents;
    uint8_t test_data[4] = {12, 23, 34, 45};
    uint8_t receiver[10];
};

TEST(RingBufferTestBasic, Instantiation)
{
    RingBuffer buffer;
    std::vector<uint8_t> contents(10);
    ASSERT_NO_THROW(ring_buffer_init(&buffer, contents.data(), contents.size()));

    ASSERT_EQ(ring_buffer_free_space(&buffer), contents.size());
    ASSERT_EQ(ring_buffer_data_size(&buffer), 0);
}

TEST_F(RingBufferTest, SimplePutGet)
{
    ASSERT_NO_THROW(ring_buffer_put_data(&buffer, test_data, sizeof(test_data)));

    ASSERT_EQ(ring_buffer_data_size(&buffer), 4);
    ASSERT_EQ(ring_buffer_free_space(&buffer), contents.size() - 4);

    size_t size = ring_buffer_data_size(&buffer);
    ASSERT_NO_THROW(ring_buffer_get_data(&buffer, receiver, size));

    ASSERT_EQ(0, memcmp(test_data, receiver, size));

    ASSERT_EQ(ring_buffer_data_size(&buffer), 0);
    ASSERT_EQ(ring_buffer_free_space(&buffer), contents.size());
}

TEST_F(RingBufferTest, CyclicPutGet)
{
    for (int i = 0; i < 100; i++)
    {
        ASSERT_NO_THROW(ring_buffer_put_data(&buffer, test_data, sizeof(test_data)));

        ASSERT_EQ(ring_buffer_data_size(&buffer), sizeof(test_data));

        ASSERT_NO_THROW(ring_buffer_put_data(&buffer, test_data, sizeof(test_data)));

        ASSERT_EQ(ring_buffer_data_size(&buffer), 2 * sizeof(test_data));

        size_t size = sizeof(test_data);
        ASSERT_NO_THROW(ring_buffer_get_data(&buffer, receiver, size));

        ASSERT_EQ(0, memcmp(test_data, receiver, size));
        ASSERT_EQ(ring_buffer_data_size(&buffer), sizeof(test_data));

        ASSERT_NO_THROW(ring_buffer_get_data(&buffer, receiver, size));

        ASSERT_EQ(0, memcmp(test_data, receiver, size));
        ASSERT_EQ(ring_buffer_data_size(&buffer), 0);
    }
}

TEST_F(RingBufferTest, AtSkip)
{
    ring_buffer_put_data(&buffer, test_data, sizeof(test_data)); // (0,4)
    ring_buffer_get_data(&buffer, receiver, sizeof(test_data));

    ring_buffer_put_data(&buffer, test_data, sizeof(test_data));  // (4,8)
    ring_buffer_put_data(&buffer, test_data, sizeof(test_data)); // (8,2)

    ASSERT_EQ(*ring_buffer_at(&buffer, 1), 23);
    ASSERT_NO_THROW(ring_buffer_skip(&buffer, 2));
    ASSERT_EQ(*ring_buffer_at(&buffer, 1), 45);
}

TEST_F(RingBufferTest, PutFromBuffer)
{
    auto linear_buf = Buffer::create();
    float test_val = 3.1415;
    *linear_buf << test_val;
    *linear_buf >> buffer;

    ASSERT_EQ(ring_buffer_data_size(&buffer), sizeof(test_val));

    auto lib_buf_2 = Buffer::create();
    *lib_buf_2 << buffer;

    ASSERT_EQ(lib_buf_2->size(), sizeof(test_val));

    float from_buffer = *reinterpret_cast<float*>(lib_buf_2->data());

    ASSERT_EQ(from_buffer, test_val);
}
