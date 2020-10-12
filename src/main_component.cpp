#include "main_component.h"
#include "components/placer.h"
#include "monitor.h"

MainComponent::MainComponent()
    : log_component_(
          [] {
            bbmp::LogMessage message;
            if (bbmp::NonBlockingLogger::GetInstance().TryDequeue(message)) {
              return std::make_optional<bbmp::LogMessage>(std::move(message));
            }
            return std::optional<bbmp::LogMessage>{};
          },
          false) {
  SetOnChangeCallback([this]() { triggerAsyncUpdate(); });
  CreatePhysicalMonitorHandles();
  addAndMakeVisible(log_component_);
  setSize(250, 100);
}

void MainComponent::handleAsyncUpdate() {
  int i_monitor = 0;
  brightness_controls_.clear();
  VisitPhysicalMonitorHandles([this, &i_monitor](PhysicalMonitor& monitor) {
    brightness_controls_.push_back(std::make_unique<BrightnessControl>());

    addAndMakeVisible(*brightness_controls_[i_monitor]);
    brightness_controls_[i_monitor]->slider_brightness_.onDragEnd =
        [this, i_monitor] {
          bool success = true;
          const auto brightness = static_cast<int>(
              brightness_controls_[i_monitor]->slider_brightness_.getValue());
          int i_monitor_inner = 0;
          VisitPhysicalMonitorHandles(
              [this, brightness, i_monitor, &success,
               &i_monitor_inner](PhysicalMonitor& monitor) {
                if (i_monitor_inner == i_monitor) {
                  success = success && monitor.SetBrightness(brightness, false);
                }
                ++i_monitor_inner;
              });
          if (!success) {
            CreatePhysicalMonitorHandles();
          }
        };

    brightness_controls_[i_monitor]->setName(monitor.GetName());
    const auto brightness = monitor.GetBrightness();
    brightness_controls_[i_monitor]->setValue(brightness);
    ++i_monitor;
  });
  setSize(250, std::max(100, i_monitor * 70));
  resized();
}

void MainComponent::paint(juce::Graphics& g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
  Placer placer(getLocalBounds());
  const auto width = getWidth();
  const auto control_height =
      getHeight() / std::max(1ull, brightness_controls_.size());
  for (auto& brightness_control : brightness_controls_) {
    placer.Place(*brightness_control, width, control_height);
  }
}
