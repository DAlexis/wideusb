#include "communication/json/helpers-json.hpp"
#include "buffer.hpp"

#include "gtest/gtest.h"

#include <iostream>

using namespace std;

class ExtractJSON : public ::testing::Test
{
protected:
    void SetUp() override {

    }

    void test_buf(const std::string& str, bool is_json)
    {
        PBuffer buf = Buffer::create(str.size(), str.data());
        BufferAccessor acc(buf);
        test_accessor(acc, is_json);
    }
    void test_ring_buf(const std::string& str, bool is_json)
    {

        RingBufferClass rb(400);
        char c = 'x';
        for (size_t i = 0; i < 390; i++)
            rb.put((const uint8_t*) &c, 1);
        rb.skip(390);

        rb.put(str.data(), str.size());
        test_accessor(rb, is_json);
    }

    void test_accessor(ISerialReadAccessor& accessor,  bool is_json)
    {
        ASSERT_NO_THROW(opt = extract_possible_json(accessor));
        if (is_json)
        {
            ASSERT_TRUE(opt.has_value());
            ASSERT_EQ(*opt, json);
        } else {
            ASSERT_FALSE(opt.has_value());
        }
    }

    std::optional<string> opt;

    std::string json = "{\"size\":28,      \"checksum\"    :     1234556     }";
    std::string not_json1 = "{\"size\":28,      \"checksum\"    :     1234556     ]";
    std::string json2 = "some readable symbols" + json + "other symbols";
    std::string json3 = "\0====\r\n\0\0" + json + "\0\0\0";

    std::string not_json2 = "{" + json;
    std::string json4 = "\0\r\n\r" + json + "dsadfds\0\0\n\r\t" + json + "||\\\0\0\0";
};

TEST_F(ExtractJSON, CommonBufferExtraction)
{
    test_buf(json, true);
    test_buf(json2, true);
    test_buf(json3, true);
    test_buf(json4, true);
    test_buf(not_json1, false);
    test_buf(not_json2, false);
}

TEST_F(ExtractJSON, RingBufferExtraction)
{
    test_ring_buf(json, true);
    test_ring_buf(json2, true);
    test_ring_buf(json3, true);
    test_ring_buf(json4, true);
    test_ring_buf(not_json1, false);
    test_ring_buf(not_json2, false);
}
