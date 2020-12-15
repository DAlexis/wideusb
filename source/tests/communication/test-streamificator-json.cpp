#include "communication/json/streamificator.hpp"

#include "gtest/gtest.h"

#include "ring-buffer.h"

#include <iostream>

TEST(StreamificationJSON, ConsistentStream)
{
    StreamificatorJSON out;
    DestreamificatorJSON in;

    const char some_data[] = "Some data here";

    PBuffer data = Buffer::create(sizeof(some_data), some_data);

    std::vector<uint8_t> ring_contents(100);
    RingBuffer rb;
    ring_buffer_init(&rb, ring_contents.data(), ring_contents.size());

    out.pack(rb, data);

    auto unpacked = in.unpack(rb);
    ASSERT_TRUE(unpacked.has_value());

    PBuffer buf = *unpacked;
    ASSERT_EQ(*data, *buf);
}
