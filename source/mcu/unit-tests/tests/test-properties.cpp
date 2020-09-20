#include "tools/json-objects.hpp"

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
        doc.Parse("{\"host_object\": {\"int_par_1\" : 123, \"int_par_2\": 234} }");
        /*doc.SetObject();
        auto & alloc = doc.GetAllocator();

        Value host_object(kObjectType);
        ASSERT_TRUE(host_object.IsObject());

        host_object.AddMember("int_par_1", Value(123), alloc);
        host_object.AddMember("int_par_2", Value(234), alloc);

        ASSERT_TRUE(host_object.IsObject());
        doc.AddMember("host_object", host_object, alloc);*/
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

TEST(ActionSelectorClass, BasicUsage)
{
    ObjectFilter selector("action");

    bool thing1_done = false, thing2_done = false;

    selector.add("thing_1", [&thing1_done](const rapidjson::Value&) { thing1_done = true; return std::optional<std::string>(); });
    selector.add("thing_2", [&thing2_done](const rapidjson::Value&) { thing2_done = true; return std::optional<std::string>(); });

    {
        thing1_done = false, thing2_done = false;

        Document doc;
        doc.Parse("{\"action\" : \"thing_1\"}");

        selector.receive_object(doc);
        ASSERT_TRUE(thing1_done);
        ASSERT_FALSE(thing2_done);
    }

    {
        thing1_done = false, thing2_done = false;

        Document doc;
        doc.Parse("{\"action\" : \"thing_2\"}");

        selector.receive_object(doc);
        ASSERT_FALSE(thing1_done);
        ASSERT_TRUE(thing2_done);
    }

    {
        thing1_done = false, thing2_done = false;

        Document doc;
        doc.Parse("{\"action\" : \"thing_XXX\"}");

        const auto res = selector.receive_object(doc);
        ASSERT_TRUE(res.has_value());

        ASSERT_FALSE(thing1_done);
        ASSERT_FALSE(thing2_done);
    }

    {
        thing1_done = false, thing2_done = false;

        Document doc;
        doc.Parse("{\"action_NO_ACTION\" : \"thing_1\"}");

        const auto res = selector.receive_object(doc);
        ASSERT_TRUE(res.has_value());

        ASSERT_FALSE(thing1_done);
        ASSERT_FALSE(thing2_done);
    }
}

TEST(ModuleActionProperties, TypicalUsage)
{

    ObjectFilter module_filter("module");
    ObjectFilter action_filter("action");
    PropertiesCollection prop_col;

    Property<int> val1(0);
    prop_col.add("val1", &val1);
    prop_col.add_filter(action_filter);

    action_filter.add_action("core", module_filter);

    {
        const char json[] = "{"
            "\"module\" : \"core\","
            "\"action\" : \"setup\","
            "\"properties\" : {\"val1\" : 123}"
        "}";

        Document doc;
        doc.Parse(json);
        auto result = module_filter.receive_object(doc);
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(int(val1), 123);
    }
    {
        const char json[] = "{"
            "\"module\" : \"core\","
            "\"action\" : \"setup_XX\","
            "\"properties\" : {\"val1\" : 123}"
        "}";

        Document doc;
        doc.Parse(json);
        auto result = module_filter.receive_object(doc);
        ASSERT_TRUE(result.has_value());
    }
    {
        const char json[] = "{"
            "\"module\" : \"NOT_core\","
            "\"action\" : \"setup\","
            "\"properties\" : {\"val1\" : 123}"
        "}";

        Document doc;
        doc.Parse(json);
        auto result = module_filter.receive_object(doc);
        ASSERT_TRUE(result.has_value());
    }
    {
        const char json[] = "{"
            "\"module\" : \"core\","
            "\"action\" : \"setup\","
            "\"properties\" : {\"val_OTHER\" : 123}"
        "}";

        Document doc;
        doc.Parse(json);
        auto result = module_filter.receive_object(doc);
        ASSERT_FALSE(result.has_value());
    }
}
