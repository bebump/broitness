#pragma once

#include "Device.h"

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include <functional>
#include <memory>
#include <utility>

struct PhysicalMonitor
{
public:
    enum class Source
    {
        dp1 = 15,
        dp2OrUsbC = 16,
        hdmi1 = 17,
        hdmi2 = 18
    };

    struct Impl;

    PhysicalMonitor (std::unique_ptr<Impl>&&);

    ~PhysicalMonitor();

    PhysicalMonitor (PhysicalMonitor&&);

    const juce::String& getName();

    int getBrightness();

    /* Using force == true will send a command to the monitor even if the program
       state indicates, that the brightness already equals the requested
       brightness.

       Returns true if the operation was successful. If it failed, you should call
       createPhysicalMonitorHandles to recreate the handles.
    */
    bool setBrightness (int brightness, bool force);

    bool setInput (Source source);

    std::optional<Source> getInput();

private:
    std::unique_ptr<Impl> impl;
};

std::optional<PhysicalMonitor::Source> getSourceFromSourceInt (int source);

class Monitors : private juce::Timer
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void physicalMonitorsChanged() = 0;
    };

    static Monitors& getInstance();

    ~Monitors();

    int getNumMonitors() const;
    PhysicalMonitor& getMonitor (int index);
    void recreatePhysicalMonitorHandles();

    void addListener    (Listener& l) { listeners.add (&l); }
    void removeListener (Listener& l) { listeners.remove (&l); }

    /* internal */
    void addPhysicalMonitor (PhysicalMonitor&& monitor);

    /* internal */
    void retry (bool shouldRetry);

private:
    Monitors() { recreatePhysicalMonitorHandles(); }

    void timerCallback() override;

    void displayChanged();

    std::vector<PhysicalMonitor> physicalMonitors;
    juce::ListenerList<Listener> listeners;
    DeviceChangeDetector displayChangeDetector { "Display Detector",
                                                 DeviceChangeDetector::DeviceType::display,
                                                 [this] (auto, auto) { displayChanged(); } };
    int retryCounter = 5;
};

// >>> ========================================================================
/* These are all thread-safe functions wrapped around the Win32 API providing
   access to monitor brightness control.

   If the PhysicalMonitor::setBrightness function returns false (indicating a
   failure to set brightness) you should call createPhysicalMonitorHandles to
   create new, valid handles, while automatically discarding the old ones. Doing
   so will also call the function you've supplied in setOnChangeCallback, so you
   can respond to this event.
*/
void createPhysicalMonitorHandles();

void visitPhysicalMonitorHandles (std::function<void (PhysicalMonitor&)> visitor);

void setOnChangeCallback (std::function<void()>);
// <<< ------------------------------------------------------------------------
