#pragma once

#include <juce_core/juce_core.h>

class DeviceChangeDetector
{
public:
    enum class DeviceType
    {
        hid,
        display
    };

    enum class DeviceEvent
    {
        arrival,
        removeComplete,
        displayChange
    };

    DeviceChangeDetector (juce::String name,
                          DeviceType type,
                          std::function<void(DeviceEvent, DeviceType)> cb);

    ~DeviceChangeDetector();

private:
    std::function<void(DeviceEvent, DeviceType)> callback;

    struct Impl;
    std::unique_ptr<Impl> impl;
};
