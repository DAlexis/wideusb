#include "serialization-json/core-msg-serialization.hpp"
#include "serialization-json/header-json.hpp"
#include "ring-buffer.h"

#include "gtest/gtest.h"

#include <iostream>

using namespace std;

TEST(HeaderJSON, Serialization)
{
    MessageHeader header;
    header.message_type = "core.test_message";
    header.body_size_bytes = 23;

    HeaderSerializerJSON serializer;
    PBuffer stream = serializer.serialize(header);

    // Creating ring buffer
    std::vector<uint8_t> contents(100);
    RingBuffer ring_buffer;
    ring_buffer_init(&ring_buffer, contents.data(), contents.size());

    *stream >> ring_buffer;

    HeaderDeserializerJSON deserializer;
    auto result = deserializer.deserialize(ring_buffer);
    //cout << "hi" << endl;

    //ASSERT_EQ(ring_buffer_data_size(&ring_buffer), 0);
    ASSERT_TRUE(result.has_value());

    ASSERT_EQ(result->body_size_bytes, header.body_size_bytes);
    ASSERT_EQ(result->message_type, header.message_type);
}

TEST(StatusRequestMsg, Serialization)
{
    StatusRequest src;
    src.do_blink = true;

    StatusRequest target;

    JSONSerializer<StatusRequest> serializer(src);
    PBuffer stream = serializer.serialize();

    JSONDeserializer<StatusRequest> deserializer([&target](const StatusRequest& result) { target = result; });
    deserializer.deserialize(stream);

    ASSERT_EQ(src.do_blink, target.do_blink);
}

TEST(StatusResponseMsg, Serialization)
{
    StatusResponse src;
    src.alive = true;
    src.free_mem = 123;
    src.system_ticks = 789;

    StatusResponse target;

    JSONSerializer<StatusResponse> serializer(src);
    PBuffer stream = serializer.serialize();

    JSONDeserializer<StatusResponse> deserializer([&target](const StatusResponse& result) { target = result; });
    deserializer.deserialize(stream);

    ASSERT_EQ(src.alive, target.alive);
    ASSERT_EQ(src.free_mem, target.free_mem);
    ASSERT_EQ(src.system_ticks, target.system_ticks);
}
