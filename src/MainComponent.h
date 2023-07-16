#pragma once

#include "Monitor.h"
#include "components/BrightnessControl.h"
#include "components/LogComponent.h"

#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>
#include <optional>
#include <vector>

using namespace juce;

struct RulesComponent : Component
{
    RulesComponent()
    {
        setSize (400, 300);
    }
};

struct RulesInfoComponent : Component
{
    RulesInfoComponent (ValueTree rulesIn) : rules (rulesIn)
    {
        buttonAddRules.onClick = [this]
        {
            RulesComponent rc;

            DialogWindow::LaunchOptions options;

            auto rulesComponent = std::make_unique<RulesComponent>();
            options.content.setOwned (rulesComponent.release());

            Rectangle<int> area (0, 0, 300, 200);

            options.content->setSize (area.getWidth(), area.getHeight());

            options.dialogTitle                  = "Broitness automation rules editor";
            options.dialogBackgroundColour       = Colour (0xff0e345a);
            options.escapeKeyTriggersCloseButton = true;
            options.useNativeTitleBar            = false;
            options.resizable                    = true;

            if (auto* dialogWindow = options.launchAsync())
                dialogWindow->centreWithSize (300, 200);
        };

        addAndMakeVisible (title);
        addAndMakeVisible (buttonAddRules);
        update();
    }

    void paint (Graphics& g) override
    {
        g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).contrasting (0.6f));
        g.fillRect (5, 5, getWidth() - 2 * 5, 2);
    }

    void update()
    {
        if (rules.getNumChildren() == 0)
        {
            title.setText ("No automation rules have been added yet", NotificationType::dontSendNotification);
            buttonAddRules.setButtonText ("Add rules...");
        }
        else
        {
            buttonAddRules.setButtonText ("Edit or remove automation rules...");
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (10);
        title.setBounds (bounds.removeFromTop (40));

        const auto buttonWidth = buttonAddRules.getButtonText().length() * 8;
        buttonAddRules.setBounds (bounds.removeFromTop (35).removeFromLeft (buttonWidth));
    }

    Label title;
    ValueTree rules;
    TextButton buttonAddRules { "Add rules..." };
};

class MainComponent  : public Component,
                       public Monitors::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (Graphics&) override;
    void resized() override;
    void update();

    void physicalMonitorsChanged() override;

private:
    std::vector<std::unique_ptr<BrightnessControl>> brightnessControls;
    ValueTree r;
    RulesInfoComponent rules;
    LogComponent logComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
