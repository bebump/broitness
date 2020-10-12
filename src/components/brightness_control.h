#include "slider.h"

#include <juce_gui_basics/juce_gui_basics.h>

struct BrightnessControl : public juce::Component {
  BrightnessControl();
  void resized() override;
  void setName(const juce::String& name);
  void setValue(int brightness);

  juce::Label label_monitor_name_;
  juce::Slider slider_brightness_;
};
