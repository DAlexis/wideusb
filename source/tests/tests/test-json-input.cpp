#include "rapidjson-config.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "serialization-json/ring-buffer-json.hpp"

#include "gtest/gtest.h"

#include <iostream>

using namespace std;
using namespace rapidjson;

static RingBuffer ring_buffer;

static void clear_ring_buffer(RingBuffer* ring_buffer)
{
    memset(ring_buffer, 0, sizeof(RingBuffer));
}

TEST(RapidJOSN, WorksWithSpecifiedDefines)
{
    // This test check that RapidJSON configuration specified at `rapidjson-config.h`
    // works correctly
    {
        const std::string debug_massage = "Hello, world";
        Document d;
        d.SetObject();
        auto & alloc = d.GetAllocator();

        Value module("core");
        d.AddMember("module", module, alloc);

        Value action("debug");
        d.AddMember("action", action, d.GetAllocator());

        Value body(kObjectType);
        Value message("Hello, world!");
        body.AddMember("msg", message, d.GetAllocator());

        d.AddMember("body", body, d.GetAllocator());

        const std::string id_str("test_id");

        Value id(kStringType);
        id.SetString(StringRef(id_str.c_str()));
        d.AddMember("msg_id", id, alloc);

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        d.Accept(writer);

        //cout << buffer.GetString() << endl;
    }
    {
        const char* json = "{ \"module\":\"core\", \"action\":\"get_status\"}";
        Document d;
        d.Parse(json);
        ASSERT_EQ(d["module"], "core");
    }
}

TEST(RingBufferParsing, JSONDetection1)
{
    const unsigned char sample_with_trash[] = "trash trash trash { \"module\":\"core\", \"dict\":{\"key\":\"value!11\"}}Qasdasdakjhlkjhsad";

    clear_ring_buffer(&ring_buffer);
    memcpy(ring_buffer.ring_buffer, sample_with_trash, sizeof(sample_with_trash));
    ring_buffer.p_read = 0;
    ring_buffer.p_write = sizeof(sample_with_trash);

    auto json_string = extract_possible_json(&ring_buffer);
    ASSERT_TRUE(json_string.has_value());
    ASSERT_EQ(ring_buffer.ring_buffer[ring_buffer.p_read], 'Q');
    //cout << ">" << *json_string << "<" << endl;

    auto next_json_string = extract_possible_json(&ring_buffer);
    ASSERT_FALSE(next_json_string.has_value());
    ASSERT_EQ(ring_buffer.p_read, ring_buffer.p_write);
}

TEST(RingBufferParsing, JSONDetection2)
{
    const unsigned char sample_with_trash[] = "trash { \"module\":\"core\", \"action\":\"get_status\"}Q other trash { \"module\":\"nope\" }W traaash";

    clear_ring_buffer(&ring_buffer);
    memcpy(ring_buffer.ring_buffer, sample_with_trash, sizeof(sample_with_trash));
    ring_buffer.p_read = 0;
    ring_buffer.p_write = sizeof(sample_with_trash);

    auto json_string = extract_possible_json(&ring_buffer);
    ASSERT_TRUE(json_string.has_value());
    ASSERT_EQ(ring_buffer.ring_buffer[ring_buffer.p_read], 'Q');
    //cout << ">" << *json_string << "<" << endl;

    json_string = extract_possible_json(&ring_buffer);
    ASSERT_TRUE(json_string.has_value());
    //cout << ">" << *json_string << "<" << endl;
    ASSERT_EQ(ring_buffer.ring_buffer[ring_buffer.p_read], 'W');
}

TEST(RingBufferParsing, JSONDetection3)
{
    const unsigned char sample_with_trash[] = "trash trash trash { \"module\":\"core\", \"action\":\"get_status\"}Qasdasdakjhlkjhsad";

    clear_ring_buffer(&ring_buffer);
    memcpy(ring_buffer.ring_buffer, sample_with_trash, sizeof(sample_with_trash));
    ring_buffer.p_read = 0;
    ring_buffer.p_write = 40; // ~ 't' in 'action'

    // Read pointer should move to first possible JSON beginning
    auto json_string = extract_possible_json(&ring_buffer);
    ASSERT_FALSE(json_string.has_value());
    ASSERT_EQ(ring_buffer.ring_buffer[ring_buffer.p_read], '{');

    // Read pointer should stay at the same place
    json_string = extract_possible_json(&ring_buffer);
    ASSERT_FALSE(json_string.has_value());
    ASSERT_EQ(ring_buffer.ring_buffer[ring_buffer.p_read], '{');

    // Now 'adding' some data
    ring_buffer.p_write += 10;

    // Read pointer should stay at the same place
    json_string = extract_possible_json(&ring_buffer);
    ASSERT_FALSE(json_string.has_value());
    ASSERT_EQ(ring_buffer.ring_buffer[ring_buffer.p_read], '{');

    // Now 'adding' all data
    ring_buffer.p_write = sizeof(sample_with_trash);

    // Read pointer should move after the JSON end
    json_string = extract_possible_json(&ring_buffer);
    ASSERT_TRUE(json_string.has_value());
    ASSERT_EQ(ring_buffer.ring_buffer[ring_buffer.p_read], 'Q');
    //cout << ">" << *json_string << "<" << endl;

    auto next_json_string = extract_possible_json(&ring_buffer);
    ASSERT_FALSE(next_json_string.has_value());
    ASSERT_EQ(ring_buffer.p_read, ring_buffer.p_write);
}

TEST(RingBufferParsing, JSONDetection4)
{
    const unsigned char sample_with_trash[] = "NO JSON!!!!!!";

    clear_ring_buffer(&ring_buffer);
    memcpy(ring_buffer.ring_buffer, sample_with_trash, sizeof(sample_with_trash));
    ring_buffer.p_read = 0;
    ring_buffer.p_write = 40; // ~ 't' in 'action'

    auto json_string = extract_possible_json(&ring_buffer);
    ASSERT_FALSE(json_string.has_value());
}
