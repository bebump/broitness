#include <juce_gui_basics/juce_gui_basics.h>

#include "../Logger.h"

class LogComponent  : public juce::Component
{
public:
    LogComponent();

    void resized() override;

private:
    void logChanged (const bbmp::Logger*);
    void update();

    juce::TextEditor editor;
    bbmp::RegistrationToken loggerRegistration { bbmp::Logger::getInstance().addListener ([this] (auto l)
                                                                                          {
                                                                                              logChanged (l);
                                                                                          }) };
};
