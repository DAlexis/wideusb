#include "wideusb/communication/binary/channel.hpp"

#include "gtest/gtest.h"

TEST(ChannelLayerBinaryClass, SimpleOperating)
{
    const char test_data_1[] = ">Whatever you want here<";
    const char test_data_2[] = ">Anything else<";
    uint8_t tmp[255];
    RingBuffer ring_buffer(200);
    ChannelLayerBinary channel;

    // Single frame test
    SegmentBuffer sg1(Buffer::create(sizeof(test_data_1), test_data_1));
    channel.encode(sg1);

    BufferAccessor acc1(sg1.merge());
    ring_buffer.put(acc1, acc1.size());

    std::vector<DecodedFrame> frames = channel.decode(ring_buffer);

    ASSERT_EQ(frames.size(), 1);
    ASSERT_EQ(frames[0].frame.size(), sizeof(test_data_1));
    frames[0].frame.extract(tmp, frames[0].frame.size());

    ASSERT_EQ(0, memcmp(test_data_1, tmp, frames[0].frame.size()));

    // 2 frames test
    SegmentBuffer sg2(Buffer::create(sizeof(test_data_2), test_data_2));
    channel.encode(sg2);

    sg1.push_back(sg2);
    BufferAccessor acc2(sg1.merge());
    ring_buffer.put(acc2, acc2.size());

    frames = channel.decode(ring_buffer);
    ASSERT_EQ(frames.size(), 2);
    ASSERT_EQ(frames[0].frame.size(), sizeof(test_data_1));
    frames[0].frame.extract(tmp, frames[0].frame.size());

    ASSERT_EQ(0, memcmp(test_data_1, tmp, frames[0].frame.size()));

    ASSERT_EQ(frames[1].frame.size(), sizeof(test_data_2));
    frames[1].frame.extract(tmp, frames[1].frame.size());

    ASSERT_EQ(0, memcmp(test_data_2, tmp, frames[1].frame.size()));
}

TEST(ChannelLayerBinaryClass, FalseHeaders)
{
    const char test_data_1[] = ">Whatever you want here<";
    const char test_data_2[] = ">Anything else<";
    uint8_t tmp[255];
    RingBuffer ring_buffer(200);
    ChannelLayerBinary channel;

    ChannelHeader false_header;
    false_header.size = 5;

    // False header
    SegmentBuffer sg1;

    // Real data
    sg1.push_back(Buffer::create(sizeof(test_data_1), test_data_1));
    channel.encode(sg1);

    sg1.push_front(Buffer::create(sizeof(false_header), &false_header));

    // False header
    sg1.push_back(Buffer::create(sizeof(false_header), &false_header));
    // False header
    sg1.push_back(Buffer::create(sizeof(false_header), &false_header));

    // Real data
    SegmentBuffer sg2(Buffer::create(sizeof(test_data_2), test_data_2));
    channel.encode(sg2);

    sg1.push_back(sg2);
    BufferAccessor acc1(sg1.merge());
    ring_buffer.put(acc1, acc1.size());

    auto frames = channel.decode(ring_buffer);
    ASSERT_EQ(frames.size(), 2);
    ASSERT_EQ(frames[0].frame.size(), sizeof(test_data_1));
    frames[0].frame.extract(tmp, frames[0].frame.size());

    ASSERT_EQ(0, memcmp(test_data_1, tmp, frames[0].frame.size()));

    ASSERT_EQ(frames[1].frame.size(), sizeof(test_data_2));
    frames[1].frame.extract(tmp, frames[1].frame.size());

    ASSERT_EQ(0, memcmp(test_data_2, tmp, frames[1].frame.size()));
}

TEST(ChannelLayerBinaryClass, Corruption)
{
    const char test_data_1[] = ">Whatever you want here<";
    uint8_t tmp[255];
    RingBuffer ring_buffer(200);
    ChannelLayerBinary channel;

    const char garbage[] = "#THIS IS A GARBAGE#";

    for (size_t i = 0; i < 100; i ++)
    {
        // Single frame test
        SegmentBuffer sg1(Buffer::create(sizeof(test_data_1), test_data_1));
        channel.encode(sg1);

        ring_buffer.put(garbage, sizeof(garbage));

        BufferAccessor acc1(sg1.merge());
        ring_buffer.put(acc1, acc1.size());

        ring_buffer.put(garbage, sizeof(garbage));

        std::vector<DecodedFrame> frames = channel.decode(ring_buffer);

        ASSERT_EQ(frames.size(), 1);
        ASSERT_EQ(frames[0].frame.size(), sizeof(test_data_1));
        frames[0].frame.extract(tmp, frames[0].frame.size());

        ASSERT_EQ(0, memcmp(test_data_1, tmp, frames[0].frame.size()));
    }
}
