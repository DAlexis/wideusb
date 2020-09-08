#include "tools/property-map.hpp"

#include "rapidjson-config.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "gtest/gtest.h"

#include <iostream>

using namespace rapidjson;
using namespace std;

TEST(PropertyClass, BasicUsage)
{
    Property<int> p;
    p << 25;
    p += 1;
    ASSERT_EQ(int(p), 26);
}


TEST(PropertiesCollectionClass, BasicUsage)
{
    Property<int> p1(25), p2(13);
    PropertiesCollection pc;

    ASSERT_EQ(int(p1), 25);
    ASSERT_EQ(int(p2), 13);

    pc.add("int_par_1", &p1);
    pc.add("int_par_2", &p2);

    {
        Document doc;
        doc.SetObject();
        auto & alloc = doc.GetAllocator();

        Value host_object(kObjectType);
        ASSERT_TRUE(host_object.IsObject());

        host_object.AddMember("int_par_1", Value(123), alloc);
        host_object.AddMember("int_par_2", Value(234), alloc);

        ASSERT_TRUE(host_object.IsObject());
        doc.AddMember("host_object", host_object, alloc);
        ASSERT_TRUE(doc["host_object"].IsObject());

        auto result = pc.receive_object(doc["host_object"]);

        if (result.has_value())
        {
            cout << result.value() << endl;
        }

        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(int(p1), 123);
        ASSERT_EQ(int(p2), 234);
    }

    p1 << -12; p2 << -44;

    {
        Document doc;
        doc.SetObject();
        auto & alloc = doc.GetAllocator();
        doc.AddMember("generated_object", pc.construct_object(alloc), alloc);

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        doc.Accept(writer);

        //cout << buffer.GetString() << endl;

        ASSERT_EQ(doc["generated_object"].GetObject()["int_par_1"].GetInt(), -12);
        ASSERT_EQ(doc["generated_object"].GetObject()["int_par_2"].GetInt(), -44);
    }
}
