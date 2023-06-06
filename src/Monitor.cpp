#include "Monitor.h"

std::optional<PhysicalMonitor::Source> getSourceFromSourceInt (int source)
{
    switch (source)
    {
        case (int) PhysicalMonitor::Source::hdmi1:
            return PhysicalMonitor::Source::hdmi1;

        case (int) PhysicalMonitor::Source::hdmi2:
            return PhysicalMonitor::Source::hdmi2;

        case (int) PhysicalMonitor::Source::dp1:
            return PhysicalMonitor::Source::dp1;

        case (int) PhysicalMonitor::Source::dp2OrUsbC:
            return PhysicalMonitor::Source::dp2OrUsbC;
    }

    jassertfalse;
    return {};
}

//==============================================================================
Monitors& Monitors::getInstance()
{
    static Monitors instance;
    return instance;
}

Monitors::~Monitors()
{
    stopTimer();
}

int Monitors::getNumMonitors() const
{
    return (int) physicalMonitors.size();
}

PhysicalMonitor& Monitors::getMonitor (int index)
{
    return physicalMonitors[(size_t) index];
}

void Monitors::addPhysicalMonitor (PhysicalMonitor&& monitor)
{
    physicalMonitors.push_back (std::move (monitor));
    listeners.call ([] (auto& l) { l.physicalMonitorsChanged(); });
}
