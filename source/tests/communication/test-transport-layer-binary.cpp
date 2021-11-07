#include "wideusb-common/communication/binary/transport.hpp"

#include "gtest/gtest.h"

TEST(TransportLayerBinaryClass, Operating)
{
    const char test_data_1[] = ">Whatever you want here<";
    SegmentBuffer sg1(Buffer::create(sizeof(test_data_1), test_data_1));

    const char test_data_2[] = ">Anything else<";
    SegmentBuffer sg2(Buffer::create(sizeof(test_data_2), test_data_2));

    TransportLayerBinary trans_layer;
    trans_layer.encode(sg1, 123, 0xABC, true, false, 0);

    trans_layer.encode(sg2, 321, 0xDEF, false, true, 0xABC);

    sg1.push_back(sg2);
    std::vector<DecodedSegment> segments = trans_layer.decode(BufferAccessor(sg1.merge()));
    ASSERT_EQ(segments.size(), 2);
    ASSERT_EQ(segments[0].port, 123);
    ASSERT_EQ(segments[0].segment_id, 0xABC);
    ASSERT_EQ(segments[0].ack_for_segment_id, 0);
    ASSERT_TRUE(segments[0].flags & DecodedSegment::Flags::need_ack);
    ASSERT_FALSE(segments[0].flags & DecodedSegment::Flags::is_ack);
    ASSERT_EQ(segments[0].segment.size(), sizeof(test_data_1));
    PBuffer data1 = Buffer::create(segments[0].segment);
    ASSERT_EQ(0, memcmp(data1->data(), test_data_1, sizeof(test_data_1)));

    ASSERT_EQ(segments[1].port, 321);
    ASSERT_EQ(segments[1].segment_id, 0xDEF);
    ASSERT_EQ(segments[1].ack_for_segment_id, 0xABC);
    ASSERT_FALSE(segments[1].flags & DecodedSegment::Flags::need_ack);
    ASSERT_TRUE(segments[1].flags & DecodedSegment::Flags::is_ack);
    ASSERT_EQ(segments[1].segment.size(), sizeof(test_data_2));
    PBuffer data2 = Buffer::create(segments[1].segment);
    ASSERT_EQ(0, memcmp(data2->data(), test_data_2, sizeof(test_data_2)));
}
