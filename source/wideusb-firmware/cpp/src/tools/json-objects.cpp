#include "tools/json-objects.hpp"

using namespace rapidjson;

ObjectFilter::ObjectFilter(const char* select_string) :
    m_filter_by(select_string)
{
}

void ObjectFilter::add(const char* action_name, Action action_func)
{
    m_actions[action_name] = action_func;
}

void ObjectFilter::add_self_as_action(const char* name, ObjectFilter& parent_selector)
{
    parent_selector.add(
        name,
        [this](const rapidjson::Value& object) -> ErrorCode
        {
            return receive_object(object);
        }
    );
}

ErrorCode ObjectFilter::receive_object(const rapidjson::Value& object)
{
    if (!object.IsObject())
        return ErrorCode("'object' value required to parse");

    if (!object.HasMember(m_filter_by.c_str()))
    {
        return std::string("Object has no ") + m_filter_by + " key";
    }

    if (!object[m_filter_by.c_str()].IsString())
    {
        return std::string("Object's field ") + m_filter_by + " is not a string";
    }

    const char* value = object[m_filter_by.c_str()].GetString();

    auto it = m_actions.find(value);
    if (it == m_actions.end())
    {
        return std::string("Unsupported action case: ") + value;
    }

    return it->second(object);
}


ErrorCode PropertiesCollection::receive_object(const rapidjson::Value& object)
{
    std::string result;
    if (!object.IsObject())
        return ErrorCode("'object' value required to parse");
    for (Value::ConstMemberIterator itr = object.GetObject().MemberBegin();
        itr != object.GetObject().MemberEnd(); ++itr)
    {
        auto jt = m_properties.find(itr->name.GetString());
        if (jt == m_properties.end())
        {
            result += itr->name.GetString();
            result += " is invalid property; ";
            continue;
        }
        if (! jt->second->set(itr->value))
        {
            result += itr->name.GetString();
            result += ": parsing error; ";
        }
    }
    return std::nullopt;
}

rapidjson::Value PropertiesCollection::construct_object(rapidjson::RAPIDJSON_DEFAULT_ALLOCATOR& allocator)
{
    Value host_object(kObjectType);

    for (const auto &it : m_properties)
    {
        host_object.AddMember(StringRef(it.first.c_str()), it.second->get(), allocator);
    }

    return host_object;
}


PropertiesCollection& PropertiesCollection::add(const char* name, IPropertyAccessor* property)
{
    m_properties[name] = property;
    return *this;
}

void PropertiesCollection::add_filter(ObjectFilter& selector)
{
    selector.add(
        "setup",
        [this](const rapidjson::Value& object) -> ErrorCode
        {
            if (!object.HasMember("properties"))
            {
                return "'setup' artion message should have 'properties' section";
            }
            return receive_object(object["properties"]);
        }
    );
}

Property<int>::Property(int value, SetCallback set_callback) :
    PropertyBase(set_callback)
{
    m_value = value;
}

bool Property<int>::set(const rapidjson::Value& value)
{
    if (!value.IsInt())
        return false;

    m_value = value.GetInt();
    if (m_set_callback)
        m_set_callback(m_value);
    return true;
}

rapidjson::Value Property<int>::get() const
{
    return rapidjson::Value(m_value);
}

Property<float>::Property(int value, SetCallback set_callback) :
    PropertyBase(set_callback)
{
    m_value = value;
}

bool Property<float>::set(const rapidjson::Value& value)
{
    if (!value.IsFloat())
        return false;

    m_value = value.GetFloat();
    if (m_set_callback)
        m_set_callback(m_value);
    return true;
}

rapidjson::Value Property<float>::get() const
{
    return rapidjson::Value(m_value);
}

Property<std::string>::Property(int value, SetCallback set_callback) :
    PropertyBase(set_callback)
{
    m_value = value;
}

bool Property<std::string>::set(const rapidjson::Value& value)
{
    if (!value.IsString())
        return false;

    m_value = value.GetString();
    if (m_set_callback)
        m_set_callback(m_value);
    return true;
}

rapidjson::Value Property<std::string>::get() const
{
    rapidjson::Value result(kStringType);
    result.SetString(StringRef(m_value.c_str()));
    return result;
}
