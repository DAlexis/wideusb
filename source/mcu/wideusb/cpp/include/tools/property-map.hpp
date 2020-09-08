#ifndef PROPERTYMAP_HPP_INCLUDED
#define PROPERTYMAP_HPP_INCLUDED

#include <map>
#include <string>
#include <memory>
#include <optional>
#include <functional>

#include "rapidjson-config.h"
#include "rapidjson/document.h"

class ActionsCollection
{
public:
    using Action = std::function<void(void)>;

    bool call(const char* action_name);
    void add(const char* action_name, Action action_func);

private:
    std::map<std::string, Action> m_actions;
};

class IPropertyAccessor
{
public:
    virtual bool set(const rapidjson::Value& value) = 0;
    virtual rapidjson::Value get() const = 0;
    virtual ~IPropertyAccessor() = default;
};

template<typename T>
class PropertyBase : public IPropertyAccessor
{
public:
    using SetCallback = std::function<void(const T&)>;

    PropertyBase(SetCallback set_callback) :
        m_set_callback(set_callback)
    { }

    operator T&() {
        return m_value;
    }

    operator const T&() const {
        return m_value;
    }

    PropertyBase& operator<<(const T& new_value)
    {
        m_value = new_value;
        return *this;
    }

protected:
    SetCallback m_set_callback;
    T m_value;
};

template<typename T>
class Property : public PropertyBase<T>
{
};

template<>
class Property<int> : public PropertyBase<int>
{
public:
    Property(int value = 0, SetCallback set_callback = nullptr);
    bool set(const rapidjson::Value& value) override;
    rapidjson::Value get() const override;
};

class PropertiesCollection
{
public:
    std::optional<std::string> receive_object(const rapidjson::Value& object);
    rapidjson::Value construct_object(rapidjson::RAPIDJSON_DEFAULT_ALLOCATOR& allocator);
    PropertiesCollection& add(const char* name, IPropertyAccessor* property);

private:
    std::map<std::string, IPropertyAccessor*> m_properties;
};

#endif // PROPERTYMAP_HPP_INCLUDED
