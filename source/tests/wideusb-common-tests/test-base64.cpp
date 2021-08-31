
#include "wideusb-common/utils/base64.hpp"

#include "gtest/gtest.h"

#include <iostream>

using namespace std;

TEST(Base64, EncodeDecode)
{
    {
        struct test_struct
        {
            int a = 0;
            float b = 0;
            bool d = false;
            bool operator==(const test_struct& right) const
            {
                return (a == right.a && b == right.b && d == right.d);
            }
        };

        test_struct t1 = {6, 3.14, true};
        test_struct t2;

        auto encoded = base64_encode(reinterpret_cast<const uint8_t *>(&t1), sizeof(t1));
        ASSERT_TRUE(encoded.has_value());
        //cout << "<" << *encoded << ">" << endl;
        auto decoded = base64_decode(*encoded);
        ASSERT_TRUE(decoded.has_value());
        memcpy(&t2, decoded->data(), sizeof(t2));
        ASSERT_EQ(t1, t2);
    }
    {
        vector<uint8_t> v = {3, 5, 24, 100};
        auto encoded = base64_encode(v);
        ASSERT_TRUE(encoded.has_value());
        //cout << "<" << encoded.value() << ">" << endl;
        auto decoded = base64_decode(*encoded);
        ASSERT_TRUE(decoded.has_value());
        ASSERT_EQ(*decoded, v);
    }
    {
        std::string str1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string str2 = "<";
        std::string str3 = "\\";
        std::string str4 = "=";
        std::string str5 = "-";
        ASSERT_TRUE(base64_decode(str1).has_value());
        ASSERT_FALSE(base64_decode(str2).has_value());
        ASSERT_FALSE(base64_decode(str3).has_value());
        ASSERT_FALSE(base64_decode(str4).has_value());
        ASSERT_FALSE(base64_decode(str5).has_value());
    }
}
