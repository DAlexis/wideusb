#include "communication/json/streamificator.hpp"

#include "gtest/gtest.h"

#include "ring-buffer.h"

#include <iostream>

class StreamificationJSON : public ::testing::Test
{
protected:
    void SetUp() override {
        some_data_buf = Buffer::create(sizeof(some_data), some_data);
        header_attack_buf = Buffer::create(sizeof(some_data), some_data);
    }

    void init_ring_buffer(size_t size)
    {
        ring_contents.resize(size);
        ring_buffer_init(&rb, ring_contents.data(), ring_contents.size());
    }

    StreamificatorJSON out;
    DestreamificatorJSON in;

    const char some_data[17] = ">Some data here<";
    const char header_attack[28] = "{\"size\":17, \"checksum\":123}";

    PBuffer some_data_buf;
    PBuffer header_attack_buf;

    std::vector<uint8_t> ring_contents;
    RingBuffer rb;

};

TEST_F(StreamificationJSON, ConsistentStreamSingleMsg)
{
    init_ring_buffer(100);

    out.pack(rb, some_data_buf);

    auto unpacked = in.unpack(rb);
    ASSERT_TRUE(unpacked.has_value());

    PBuffer buf = *unpacked;
    ASSERT_EQ(*some_data_buf, *buf);
}

TEST_F(StreamificationJSON, ConsistentStreamMultipleMsgs)
{
    init_ring_buffer(200);
    for (int global = 0; global < 100; global++)
    {
        // Putting data until the buffer is filled
        int count = 0;
        while (out.pack(rb, some_data_buf))
        {
            count++;
        }

        for (int i = 0; i < count; i++)
        {
            auto unpacked = in.unpack(rb);
            ASSERT_TRUE(unpacked.has_value());

            PBuffer buf = *unpacked;
            ASSERT_EQ(*some_data_buf, *buf);
        }

        auto unpacked = in.unpack(rb);
        ASSERT_FALSE(unpacked.has_value());
    }
}

TEST_F(StreamificationJSON, ConsistentStreamParsingPartByPart)
{
    init_ring_buffer(200);
    out.pack(rb, some_data_buf);
    uint32_t real_p_write = rb.p_write;
    rb.p_write = rb.p_read;

    for (rb.p_write = rb.p_read; rb.p_write < real_p_write; rb.p_write++)
    {
        auto unpacked = in.unpack(rb);
        ASSERT_FALSE(unpacked.has_value());
    }
    rb.p_write = real_p_write;
    auto unpacked = in.unpack(rb);
    ASSERT_TRUE(unpacked.has_value());

    PBuffer buf = *unpacked;
    ASSERT_EQ(*some_data_buf, *buf);
}

TEST_F(StreamificationJSON, InconsistentStreamGarbage)
{
    init_ring_buffer(1000);

    out.pack(rb, some_data_buf);
    uint8_t garbage[256];
    for (int i = 0; i < 256; i++)
        garbage[i] = (uint8_t) i;

    ring_buffer_put_data(&rb, garbage, 256); // Garbage includes {}

    out.pack(rb, some_data_buf);


    auto unpacked_first = in.unpack(rb);
    ASSERT_TRUE(unpacked_first.has_value());

    PBuffer buf = *unpacked_first;
    ASSERT_EQ(*some_data_buf, *buf);

    buf.reset();

    bool parsed_finally = false;
    for (int i = 0; i < 10; i++)
    {
        auto unpacked = in.unpack(rb);
        if (unpacked.has_value())
        {
            buf = *unpacked;
            parsed_finally = true;
            break;
        }
    }
    ASSERT_TRUE(parsed_finally);
    ASSERT_EQ(*some_data_buf, *buf);
}

TEST_F(StreamificationJSON, InconsistentCorrupted)
{
    init_ring_buffer(1000);
    out.pack(rb, some_data_buf);
    uint32_t p_write = rb.p_write;
    out.pack(rb, some_data_buf);
    rb.ring_buffer[p_write] = '|'; // Changing '{'
    out.pack(rb, some_data_buf);

    int unpacked_count = 0;

    for (int i = 0; i < 10; i++)
    {
        auto unpacked = in.unpack(rb);
        if (unpacked.has_value())
        {
            ASSERT_EQ(*some_data_buf, **unpacked);
            unpacked_count++;
        }
    }

    ASSERT_EQ(unpacked_count, 2);
}
