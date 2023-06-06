#include "../Monitor.h"

struct PhysicalMonitor::Impl
{
    Impl ()
    {
    }

    const juce::String& GetName()
    {
        return name;
    }

    int GetBrightness()
    {
        return 0;
    }

    bool SetBrightness (int newBrightness, bool force)
    {
        return false;
    }

    bool setInput (int inputId)
    {
        return false;
    }

    std::optional<Source> getInput()
    {
        return std::nullopt;
    }

    juce::String name;
};

void Monitors::recreatePhysicalMonitorHandles()
{
}

#include "MonitorCommon.cpp"
