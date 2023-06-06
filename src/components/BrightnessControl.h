#include <juce_gui_basics/juce_gui_basics.h>

class BrightnessControl  : public juce::Component
{
public:
    BrightnessControl();

    void setControlName (const juce::String& name);
    void setValue (int brightness);

    void resized() override;

    juce::Label labelMonitorName;
    juce::Slider sliderBrightness;
};
