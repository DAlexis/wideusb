#include "buffer.hpp"
#include "ring-buffer.h"

#include "gtest/gtest.h"

TEST(BufferClass, Instantiating)
{
    uint8_t test_data[4] = {12, 23, 34, 45};
    {
        PBuffer b;
        ASSERT_NO_THROW(b = Buffer::create());

        ASSERT_EQ(b->size(), 0);
    }
    {
        PBuffer b;
        ASSERT_NO_THROW(b = Buffer::create(4, test_data));

        ASSERT_EQ(b->size(), 4);
        ASSERT_EQ(0, memcmp(test_data, b->data(), 4));
    }
    {
        PBuffer b;
        ASSERT_NO_THROW(b = Buffer::create(3));

        ASSERT_EQ(b->size(), 3);
        ASSERT_EQ(0, b->data()[0]);
        ASSERT_EQ(0, b->data()[1]);
        ASSERT_EQ(0, b->data()[2]);
    }
}

TEST(BufferClass, Operating)
{
    PBuffer b1 = Buffer::create();

    uint8_t test_data1[] = {12, 23, 34, 45};
    uint8_t test_data2[] = {98, 87, 76, 65};

    b1->append(test_data1, 4);
    b1->append(test_data1, 4);

    ASSERT_EQ(b1->size(), 8);
    ASSERT_EQ(0, memcmp(test_data1, b1->data(), 4));
    ASSERT_EQ(0, memcmp(test_data1, b1->data() + 4, 4));

    PBuffer b2 = Buffer::create(4, test_data2);

    *b1 << *b2;
    ASSERT_EQ(b2->size(), 4);
    ASSERT_EQ(b1->size(), 12);
    ASSERT_EQ(0, memcmp(test_data2, b1->data()+8, 4));

    *b1 << std::move(*b2);
    ASSERT_EQ(b2->size(), 0);
    ASSERT_EQ(b1->size(), 16);
    ASSERT_EQ(0, memcmp(test_data2, b1->data()+12, 4));
}
