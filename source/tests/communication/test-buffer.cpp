#include "buffer.hpp"

#include "communication/networking.hpp"

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

    b1->put(test_data1, 4);
    b1->put(test_data1, 4);

    ASSERT_EQ(b1->size(), 8);
    ASSERT_EQ(0, memcmp(test_data1, b1->data(), 4));
    ASSERT_EQ(0, memcmp(test_data1, b1->data() + 4, 4));

    PBuffer b2 = Buffer::create(4, test_data2);

    *b1 << BufferAccessor(b2);
    ASSERT_EQ(b2->size(), 4);
    ASSERT_EQ(b1->size(), 12);
    ASSERT_EQ(0, memcmp(test_data2, b1->data()+8, 4));

    *b1 << BufferAccessor(b2);
    //ASSERT_EQ(b2->size(), 0);
    ASSERT_EQ(b1->size(), 16);
    ASSERT_EQ(0, memcmp(test_data2, b1->data()+12, 4));

    PBuffer b3 = Buffer::create();
    float pi = 3.14;
    b3->raw() << int(25) << pi << "qwer";
    ASSERT_EQ(b3->size(), sizeof(int) + sizeof(pi) + 5);
}

TEST(BufferAccessorClass, Operating)
{
    PBuffer b1 = Buffer::create();

    uint8_t test_data1[] = {12, 23, 34, 45, 56, 67, 78};
    b1->put(test_data1, sizeof(test_data1));
    BufferAccessor acc1(b1, 0, 6);
    ASSERT_EQ(acc1[0], 12);
    acc1.skip(1);
    ASSERT_EQ(acc1[0], 23);
    ASSERT_EQ(acc1.size(), 5);

    BufferAccessor acc2(acc1, 1, 7);
    ASSERT_EQ(acc2.size(), 4);
    ASSERT_EQ(acc2[1], 45);

    BufferAccessor acc3(acc1, 1, 3);
    ASSERT_EQ(acc3.size(), 2);
}

TEST(SegmentBufferClass, Operating)
{
    SegmentBuffer seg_buf;
    PBuffer b1 = Buffer::create();
    b1->raw() << 'q' << 'w' << 'e';
    PBuffer b2 = Buffer::create();
    b2->raw() << 'r' << 't';
    PBuffer b3 = Buffer::create();
    b3->raw() << 'y';

    seg_buf.push_back(b2);
    seg_buf.push_front(b1);
    seg_buf.push_back(b3);
    PBuffer result = seg_buf.merge();

    ASSERT_EQ(result->data()[0], 'q');
    ASSERT_EQ(result->data()[1], 'w');
    ASSERT_EQ(result->data()[2], 'e');
    ASSERT_EQ(result->data()[3], 'r');
    ASSERT_EQ(result->data()[4], 't');
    ASSERT_EQ(result->data()[5], 'y');
}
