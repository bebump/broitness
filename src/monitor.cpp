#include "monitor.h"

#define NOMINMAX
#include <Highlevelmonitorconfigurationapi.h>
#include <Physicalmonitorenumerationapi.h>
#include <windows.h>
#undef NOMINMAX

#include <mutex>
#include <vector>

auto physical_monitor_destructor = [](void* monitor) {
  DestroyPhysicalMonitor(monitor);
};

using MonitorHandle =
    std::unique_ptr<void, decltype(physical_monitor_destructor)>;

struct PhysicalMonitor::Impl {
  Impl(MonitorHandle&& handle, juce::String name, DWORD brightness)
      : monitor_handle_(std::move(handle)),
        monitor_name_(std::move(name)),
        brightness_(brightness) {}

  const juce::String& GetName() { return monitor_name_; }

  int GetBrightness() { return brightness_; }

  bool SetBrightness(int brightness, bool force) {
    if (brightness_ != brightness || force) {
      brightness_ = brightness;
      return SetMonitorBrightness(monitor_handle_.get(), brightness);
    }
    return true;
  }

  MonitorHandle monitor_handle_;
  juce::String monitor_name_;
  DWORD brightness_;
};

PhysicalMonitor::~PhysicalMonitor() = default;

PhysicalMonitor::PhysicalMonitor(PhysicalMonitor&&) = default;

PhysicalMonitor::PhysicalMonitor(std::unique_ptr<PhysicalMonitor::Impl>&& impl)
    : impl_(std::move(impl)) {}

const juce::String& PhysicalMonitor::GetName() { return impl_->GetName(); }

int PhysicalMonitor::GetBrightness() { return impl_->GetBrightness(); }

bool PhysicalMonitor::SetBrightness(int brightness, bool force) {
  return impl_->SetBrightness(brightness, force);
}

struct PhysicalMonitorGlobalState {
  std::vector<PhysicalMonitor> physical_monitors;
  std::mutex mutex_physical_monitors_;
  std::function<void()> on_change_callback_;
  std::mutex mutex_on_change_callback_;
};

static PhysicalMonitorGlobalState physicalMonitorGlobalStateObject;

BOOL Monitorenumproc(HMONITOR hMonitor, HDC /* hDeviceContext */,
                     LPRECT /* Arg3 */, LPARAM /* Arg4 */) {
  DWORD num_physical_monitors = 0;
  GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &num_physical_monitors);
  std::vector<PHYSICAL_MONITOR> monitors(num_physical_monitors);
  GetPhysicalMonitorsFromHMONITOR(
      hMonitor, std::max(num_physical_monitors, num_physical_monitors),
      monitors.data());

  bool change = false;
  for (const auto& monitor : monitors) {
    {
      MonitorHandle monitor_handle(monitor.hPhysicalMonitor,
                                   physical_monitor_destructor);
      DWORD min, current, maximum = 0;
      if (TRUE == GetMonitorBrightness(monitor.hPhysicalMonitor, &min, &current,
                                       &maximum)) {
        std::lock_guard lock(
            physicalMonitorGlobalStateObject.mutex_physical_monitors_);
        physicalMonitorGlobalStateObject.physical_monitors.push_back(
            PhysicalMonitor(std::make_unique<PhysicalMonitor::Impl>(
                std::move(monitor_handle), monitor.szPhysicalMonitorDescription,
                current)));
        change = true;
      }
    }
  }

  {
    std::lock_guard lock(
        physicalMonitorGlobalStateObject.mutex_on_change_callback_);
    if (physicalMonitorGlobalStateObject.on_change_callback_) {
      physicalMonitorGlobalStateObject.on_change_callback_();
    }
  }

  return TRUE;
}

void CreatePhysicalMonitorHandles() {
  {
    std::lock_guard lock(
        physicalMonitorGlobalStateObject.mutex_physical_monitors_);
    physicalMonitorGlobalStateObject.physical_monitors.clear();
  }

  {
    std::lock_guard lock(
        physicalMonitorGlobalStateObject.mutex_on_change_callback_);
    if (physicalMonitorGlobalStateObject.on_change_callback_) {
      physicalMonitorGlobalStateObject.on_change_callback_();
    }
  }

  EnumDisplayMonitors(NULL, NULL, &Monitorenumproc, NULL);
}

void VisitPhysicalMonitorHandles(
    std::function<void(PhysicalMonitor&)> visitor) {
  std::lock_guard lock(
      physicalMonitorGlobalStateObject.mutex_physical_monitors_);
  for (auto& handle : physicalMonitorGlobalStateObject.physical_monitors) {
    visitor(handle);
  }
}

void SetOnChangeCallback(std::function<void()> callback) {
  std::lock_guard lock(
      physicalMonitorGlobalStateObject.mutex_on_change_callback_);
  physicalMonitorGlobalStateObject.on_change_callback_ = std::move(callback);
}