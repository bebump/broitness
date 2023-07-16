#include "MainComponent.h"
#include "Monitor.h"

using namespace juce;

MainComponent::MainComponent() : rules (r)
{
    Monitors::getInstance().addListener (*this);
    update();
    addAndMakeVisible (rules);
    addAndMakeVisible (logComponent);
    setSize (320, 100 + 300);
}

MainComponent::~MainComponent()
{
    Monitors::getInstance().removeListener (*this);
}

void MainComponent::physicalMonitorsChanged()
{
    update();
}

void MainComponent::update()
{
    brightnessControls.clear();

    for (int i = 0; i < Monitors::getInstance().getNumMonitors(); ++i)
    {
        brightnessControls.push_back (std::make_unique<BrightnessControl>());
        auto& control = *brightnessControls.back().get();
        addAndMakeVisible (control);
        auto& monitor = Monitors::getInstance().getMonitor (i);

        control.sliderBrightness.onDragEnd = [&control, &monitor]
        {
            const auto brightness = (int) control.sliderBrightness.getValue();

            if (! monitor.setBrightness (brightness, false))
                Monitors::getInstance().recreatePhysicalMonitorHandles();
        };

        control.setControlName (monitor.getName());
        control.setValue (monitor.getBrightness());
    }

    setSize (320, std::max (100, Monitors::getInstance().getNumMonitors() * 70) + 300);
    resized();
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    rules.setBounds (bounds.removeFromBottom (100));

    logComponent.setBounds (bounds.removeFromBottom (200));

    const auto width         = bounds.getWidth();
    const auto controlHeight = 70;

    for (auto& brightnessControl : brightnessControls)
        brightnessControl->setBounds (bounds.removeFromTop (controlHeight));
}
