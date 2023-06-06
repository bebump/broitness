#include "../Device.h"

class DeviceChangeDetector::Impl
{
};


DeviceChangeDetector::DeviceChangeDetector (juce::String name,
                                            DeviceType type,
                                            std::function<void(DeviceEvent, DeviceType)> cb)
{
}

DeviceChangeDetector::~DeviceChangeDetector()
{
}
