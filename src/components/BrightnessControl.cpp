#include "BrightnessControl.h"

BrightnessControl::BrightnessControl()
{
    sliderBrightness.setEnabled (false);
    sliderBrightness.setRange (0, 100, 1);

    addAndMakeVisible (labelMonitorName);
    addAndMakeVisible (sliderBrightness);
}

void BrightnessControl::resized()
{
    auto localBounds = getLocalBounds();
    labelMonitorName.setBounds (localBounds.removeFromTop (30).reduced (5, 0));
    auto sliderLine = localBounds.removeFromTop (30).reduced (5, 0);
    sliderBrightness.setBounds (sliderLine);
}

void BrightnessControl::setControlName (const juce::String& name)
{
    labelMonitorName.setText (name, juce::NotificationType::dontSendNotification);
}

void BrightnessControl::setValue (const int brightness)
{
    sliderBrightness.setValue (static_cast<double> (brightness),
                               juce::NotificationType::dontSendNotification);
    sliderBrightness.setEnabled (true);
}
