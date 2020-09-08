#include "tools/property-map.hpp"

using namespace rapidjson;

bool ActionsCollection::call(const char* action_name)
{
    auto it = m_actions.find(action_name);
    if (it == m_actions.end())
        return false;
    it->second();
    return true;
}

void ActionsCollection::add(const char* action_name, Action action_func)
{
    m_actions[action_name] = action_func;
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

std::optional<std::string> PropertiesCollection::receive_object(const rapidjson::Value& object)
{
    std::string result;
    if (!object.IsObject())
        return std::optional<std::string>("'object' value required to parse");
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
    return std::optional<std::string>();
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
