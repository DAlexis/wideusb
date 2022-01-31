#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <cstring>
#include <functional>
#include <memory>

template<typename T>
void zerify(T& target)
{
    memset(&target, 0, sizeof(T));
}

template<typename... CallbackArgs>
struct CallbackReceiver
{
public:
    using ptr = std::shared_ptr<CallbackReceiver<CallbackArgs...>>;

    template <typename SomeCallable>
    static std::shared_ptr<CallbackReceiver<CallbackArgs...>> create(SomeCallable callable)
    {
        std::function<void(CallbackArgs...)> func = callable;
        return std::shared_ptr<CallbackReceiver<CallbackArgs...>>(new CallbackReceiver<CallbackArgs...>(func));
    }

    void drop()
    {
        m_func = 0;
    }

    void call(CallbackArgs... args)
    {
        if (m_func)
        {
            m_func(args...);
        }
    }

    ~CallbackReceiver()
    {
        drop();
    }

private:
    CallbackReceiver(std::function<void(CallbackArgs...)> func) :
        m_func(func)
    {
    }

    std::function<void(CallbackArgs...)> m_func;
};

template<>
struct CallbackReceiver<void>
{
public:
    using ptr = std::shared_ptr<CallbackReceiver<void>>;
    template <typename SomeCallable>
    static std::shared_ptr<CallbackReceiver<void>> create(SomeCallable callable)
    {
        std::function<void(void)> func = callable;
        return std::shared_ptr<CallbackReceiver<void>>(new CallbackReceiver<void>(func));
    }

    void drop()
    {
        m_func = 0;
    }

    void call()
    {
        if (m_func)
        {
            m_func();
        }
    }

    ~CallbackReceiver()
    {
        drop();
    }

private:
    CallbackReceiver(std::function<void(void)> func) :
        m_func(func)
    {
    }

    std::function<void(void)> m_func;
};

template<typename... CallbackArgs>
class CallbackEntry
{
public:
    CallbackEntry(std::nullptr_t) { }

    CallbackEntry() { }

    CallbackEntry(std::shared_ptr<CallbackReceiver<CallbackArgs...>> receiver) :
        m_receiver(receiver)
    { }

    void call(CallbackArgs... args)
    {
        if (auto ptr = m_receiver.lock())
        {
            ptr->call(args...);
        }
    }

    bool avaliable()
    {
        if (auto ptr = m_receiver.lock())
            return true;
        else
            return false;

    }

    void drop()
    {
        m_receiver.reset();
    }

    CallbackEntry& operator=(std::shared_ptr<CallbackReceiver<CallbackArgs...>> receiver)
    {
        m_receiver = receiver;
        return *this;
    }

    CallbackEntry& operator=(std::nullptr_t)
    {
        drop();
        return *this;
    }

    void operator()(CallbackArgs... args)
    {
        call(std::forward<CallbackArgs>(args)...);
    }

private:
    std::weak_ptr<CallbackReceiver<CallbackArgs...>> m_receiver;
};

template<>
class CallbackEntry<void>
{
public:
    CallbackEntry()
    {
    }

    CallbackEntry(std::shared_ptr<CallbackReceiver<void>> receiver) :
        m_receiver(receiver)
    {
    }

    void call()
    {
        if (auto ptr = m_receiver.lock())
        {
            ptr->call();
        }
    }

    bool avaliable()
    {
        if (auto ptr = m_receiver.lock())
            return true;
        else
            return false;

    }

    CallbackEntry& operator=(std::shared_ptr<CallbackReceiver<void>> receiver)
    {
        m_receiver = receiver;
        return *this;
    }

private:
    std::weak_ptr<CallbackReceiver<void>> m_receiver;
};


#endif // UTILITIES_HPP_INCLUDED
