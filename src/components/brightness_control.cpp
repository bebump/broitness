#include "brightness_control.h"

BrightnessControl::BrightnessControl() {
  slider_brightness_.setEnabled(false);
  slider_brightness_.setRange(0, 100, 1);

  addAndMakeVisible(label_monitor_name_);
  addAndMakeVisible(slider_brightness_);
}

void BrightnessControl::resized() {
  auto local_bounds = getLocalBounds();
  label_monitor_name_.setBounds(local_bounds.removeFromTop(30).reduced(5, 0));
  slider_brightness_.setBounds(local_bounds.removeFromTop(30).reduced(5, 0));
}

void BrightnessControl::setName(const juce::String& name) {
  label_monitor_name_.setText(name,
                              juce::NotificationType::dontSendNotification);
}

void BrightnessControl::setValue(const int brightness) {
  slider_brightness_.setValue(static_cast<double>(brightness),
                              juce::NotificationType::dontSendNotification);
  slider_brightness_.setEnabled(true);
}
