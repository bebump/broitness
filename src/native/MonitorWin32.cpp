#include "../Monitor.h"

#define NOMINMAX
#include <Highlevelmonitorconfigurationapi.h>
#include <Lowlevelmonitorconfigurationapi.h>
#include <Physicalmonitorenumerationapi.h>
#include <windows.h>
#undef NOMINMAX

using MonitorHandle = std::unique_ptr<void, void (*)(void*)>;

MonitorHandle createMonitorHandle (void* monitor)
{
    return MonitorHandle { monitor, [] (void* monitor) { DestroyPhysicalMonitor (monitor); } };
}

BOOL CALLBACK Monitorenumproc (HMONITOR hMonitor, [[maybe_unused]] HDC hDeviceContext, LPRECT, LPARAM lParam)
{
    std::vector<PHYSICAL_MONITOR> monitors ([&]()
                                            {
                                                DWORD numPhysicalMonitors = 0;
                                                GetNumberOfPhysicalMonitorsFromHMONITOR (hMonitor, &numPhysicalMonitors);
                                                return numPhysicalMonitors;
                                            }());

    auto success = GetPhysicalMonitorsFromHMONITOR (hMonitor,
                                                    (DWORD) monitors.size(),
                                                    monitors.data());

    jassert (success == TRUE);

    auto* self = reinterpret_cast<Monitors*> (lParam);

    bool errorEncountered = false;

    for (const auto& monitor : monitors)
    {
        auto monitorHandle = createMonitorHandle (monitor.hPhysicalMonitor);

        if (DWORD min, current, maximum = 0;
            TRUE == GetMonitorBrightness (monitor.hPhysicalMonitor, &min, &current, &maximum))
        {
            reinterpret_cast<Monitors*> (lParam)->addPhysicalMonitor (
                PhysicalMonitor (std::make_unique<PhysicalMonitor::Impl> (std::move (monitorHandle),
                                                                          monitor.szPhysicalMonitorDescription,
                                                                          current)));
        }
        else
        {
            errorEncountered = errorEncountered || true;
        }
    }

    if (errorEncountered)
    {
        self->retry (true);
        return FALSE;
    }

    self->retry (false);
    return TRUE;
}

//==============================================================================
struct PhysicalMonitor::Impl
{
    Impl (MonitorHandle&& handle, juce::String name, DWORD brightnessIn)
        : monitorHandle (std::move (handle)), monitorName (std::move (name)), brightness (brightnessIn)
    {
    }

    const juce::String& GetName()
    {
        return monitorName;
    }

    int GetBrightness()
    {
        return brightness;
    }

    bool SetBrightness (int newBrightness, bool force)
    {
        if (std::exchange (brightness, newBrightness) != newBrightness || force)
            return SetMonitorBrightness (monitorHandle.get(), brightness);

        return true;
    }

    bool setInput (int inputId)
    {
        return SetVCPFeature (monitorHandle.get(), inputVCPCode, inputId);
    }

    std::optional<Source> getInput()
    {
        DWORD currentValue, maximumValue;

        if (GetVCPFeatureAndVCPFeatureReply (monitorHandle.get(),
                                             inputVCPCode,
                                             NULL,
                                             &currentValue,
                                             &maximumValue) == FALSE)
        {
            return {};
        }

        return getSourceFromSourceInt ((int) currentValue);
    }

    static constexpr int inputVCPCode = 0x60;

    MonitorHandle monitorHandle;
    juce::String monitorName;
    DWORD brightness;
};

void Monitors::timerCallback()
{
    if (retryCounter-- > 0)
        recreatePhysicalMonitorHandles();
    else
        stopTimer();
}

void Monitors::recreatePhysicalMonitorHandles()
{
    physicalMonitors.clear();
    listeners.call ([] (auto& l) { l.physicalMonitorsChanged(); });
    EnumDisplayMonitors (NULL, NULL, &Monitorenumproc, reinterpret_cast<LPARAM> (this));
}

void Monitors::retry (bool shouldRetry)
{
    if (shouldRetry)
        startTimer (300);
    else
        stopTimer();
}

void Monitors::displayChanged()
{
    retryCounter = 5;
    recreatePhysicalMonitorHandles();
}

#include "MonitorCommon.cpp"
