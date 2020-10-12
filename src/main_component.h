#pragma once

#include "components/brightness_control.h"
#include "components/log_component.h"

#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>
#include <optional>
#include <vector>

class MainComponent : public juce::Component, private juce::AsyncUpdater {
 public:
  MainComponent();

  void paint(juce::Graphics&) override;
  void resized() override;
  void handleAsyncUpdate() override;

 private:
  std::vector<std::unique_ptr<BrightnessControl>> brightness_controls_;
  LogComponent log_component_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
