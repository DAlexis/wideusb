#ifndef PROPERTYMAP_HPP_INCLUDED
#define PROPERTYMAP_HPP_INCLUDED

#include <map>
#include <string>
#include <memory>
#include <optional>
#include <functional>

#include "rapidjson-config.h"
#include "rapidjson/document.h"

class PropertiesCollection;

using ErrorCode = std::optional<std::string>;

class ObjectFilter
{
public:
    using Action = std::function<ErrorCode(const rapidjson::Value& object)>;

    ObjectFilter(const char* filter_by);

    /**
     * @brief Parse JSON like {filter_by: <action>, "something_other": <other things>}
     * @param object Only select_string field is matter. Depends of filter_by, all object will be given to selected callback
     * @return Error text or nothing
     */
    ErrorCode receive_object(const rapidjson::Value& object);
    void add(const char* action_name, Action action_func);

    void add_self_as_action(const char* name, ObjectFilter& parent_selector);

private:
    std::map<std::string, Action> m_actions;
    const std::string m_filter_by;
};


class IPropertyAccessor
{
public:
    virtual bool set(const rapidjson::Value& value) = 0;
    virtual rapidjson::Value get() const = 0;
    virtual ~IPropertyAccessor() = default;
};

class PropertiesCollection
{
public:
    /**
     * @brief Parse properties subtree object
     * @param object Directly properties object with {key1: value1, key2: value2}
     * @return Error text or nothing
     */
    ErrorCode receive_object(const rapidjson::Value& object);
    rapidjson::Value construct_object(rapidjson::RAPIDJSON_DEFAULT_ALLOCATOR& allocator);
    PropertiesCollection& add(const char* name, IPropertyAccessor* property);

    /**
     * @brief Add filter for value word "setup" to some object filter
     * @param selector The ObjectFilter, likely by word "action"
     */
    void add_filter(ObjectFilter& selector);

private:
    std::map<std::string, IPropertyAccessor*> m_properties;
};

template<typename T>
class PropertyBase : public IPropertyAccessor
{
public:
    using SetCallback = std::function<void(T&)>;

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

template<>
class Property<float> : public PropertyBase<float>
{
public:
    Property(int value = 0, SetCallback set_callback = nullptr);
    bool set(const rapidjson::Value& value) override;
    rapidjson::Value get() const override;
};

template<>
class Property<std::string> : public PropertyBase<std::string>
{
public:
    Property(int value = 0, SetCallback set_callback = nullptr);
    bool set(const rapidjson::Value& value) override;
    rapidjson::Value get() const override;
};


#endif // PROPERTYMAP_HPP_INCLUDED
