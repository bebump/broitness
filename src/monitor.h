#pragma once

#include <juce_core/juce_core.h>

#include <functional>
#include <memory>
#include <utility>

struct PhysicalMonitor {
  struct Impl;
  PhysicalMonitor(std::unique_ptr<Impl>&&);
  ~PhysicalMonitor();

  PhysicalMonitor(PhysicalMonitor&&);

  const juce::String& GetName();
  int GetBrightness();

  // Using force == true will send a command to the monitor even if the program
  // state indicates, that the brightness already equals the requested
  // brightness.
  //
  // Returns true if the operation was successful. If it failed, you should call
  // CreatePhysicalMonitorHandles to recreate the handles.
  bool SetBrightness(int brightness, bool force);

  std::unique_ptr<Impl> impl_;
};

// >>> ========================================================================
// These are all thread-safe functions wrapped around the Win32 API providing
// access to monitor brightness control.
//
// If the PhysicalMonitor::SetBrightness function returns false (indicating a
// failure to set brightness) you should call CreatePhysicalMonitorHandles to
// create new, valid handles, while automatically discarding the old ones. Doing
// so will also the function you've supplied in SetOnChangeCallback, so you can
// respond to this even.
void CreatePhysicalMonitorHandles();

void VisitPhysicalMonitorHandles(std::function<void(PhysicalMonitor&)> visitor);

void SetOnChangeCallback(std::function<void()>);
// <<< ------------------------------------------------------------------------