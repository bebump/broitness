#include "LogComponent.h"

LogComponent::LogComponent()
{
    editor.setMultiLine (true);
    update();
    addAndMakeVisible (editor);
}

void LogComponent::resized()
{
    editor.setBounds (getLocalBounds());
}

void LogComponent::update()
{
    editor.setText (bbmp::Logger::getInstance().getText());
}

void LogComponent::logChanged (const bbmp::Logger*)
{
    update();
}
