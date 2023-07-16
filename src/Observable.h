#pragma once

#include <juce_core/juce_core.h>

#include <functional>

namespace bbmp
{

class RegistrationToken
{
public:
    RegistrationToken() = default;

    explicit RegistrationToken (std::function<void()> unregisterCallback)
            : unregister (std::move (unregisterCallback))
    {}

    ~RegistrationToken()
    {
        reset();
    }

    RegistrationToken (const RegistrationToken&) = delete;

    RegistrationToken (RegistrationToken&& other)
    {
        reset();
        std::swap (unregister, other.unregister);
    }

    RegistrationToken& operator=(const RegistrationToken&) = delete;

    RegistrationToken& operator=(RegistrationToken&& other)
    {
        reset();
        std::swap (unregister, other.unregister);
        return *this;
    }

private:
    void reset()
    {
        if (unregister != nullptr)
            unregister();

        unregister = nullptr;
    }

    std::function<void()> unregister;
};

template <typename T>
class Observable
{
public:
    Observable() = default;
    virtual ~Observable() = default;

    RegistrationToken addListener (std::function<void(const T*)> handler)
    {
        const auto handlerId = nextListenerId++;
        listeners[handlerId] = std::move (handler);
        return RegistrationToken { [handlerId, this]() { removeListener (handlerId); } };
    }

    void callObservers() const
    {
        for (const auto& it : listeners)
            it.second (static_cast<const T*> (this));
    }

private:
    void removeListener (int64_t handlerId)
    {
        auto it = listeners.find (handlerId);

        if (it == listeners.end())
        {
            jassertfalse;
            return;
        }

        listeners.erase (it);
    }

    int64_t nextListenerId = 0;
    std::unordered_map<int64_t, std::function<void(const T*)>> listeners;

    JUCE_DECLARE_NON_COPYABLE(Observable<T>)
    JUCE_DECLARE_NON_MOVEABLE(Observable<T>)
};

} // namespace bbmp
