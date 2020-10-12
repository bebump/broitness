#pragma once

#include "../logging.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>
#include <optional>

struct LogComponent : public juce::Component, private juce::Timer {
  LogComponent(
      std::function<std::optional<bbmp::LogMessage>()> log_message_getter,
      bool has_hide_button);

  void Update();

  void timerCallback() override;

  void resized() override;

  std::function<std::optional<bbmp::LogMessage>()> log_message_getter_;
  juce::StringArray log_messages_;
  juce::TextEditor editor_;
  std::unique_ptr<juce::TextButton> button_show_hide_ = nullptr;
  unsigned int message_counter_;
};
