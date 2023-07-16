#include "BinaryData.h"
#include "Device.h"
#include "MainComponent.h"
#include "Logger.h"

#include <functional>
#include <format>

static std::string enumToString (DeviceChangeDetector::DeviceEvent event)
{
    using Enum = DeviceChangeDetector::DeviceEvent;

    if (event == Enum::arrival)
        return "arrival";

    if (event == Enum::displayChange)
        return "displayChange";

    if (event == Enum::removeComplete)
        return "removeComplete";

    jassertfalse;
    return "";
}

static std::string enumToString (DeviceChangeDetector::DeviceType type)
{
    using Enum = DeviceChangeDetector::DeviceType;

    if (type == Enum::hid)
        return "hid";

    if (type == Enum::display)
        return "display";

    jassertfalse;
    return "";
}

class GuiAppApplication : public juce::JUCEApplication
{
public:
    GuiAppApplication() = default;

    const juce::String getApplicationName() override
    {
        return JUCE_APPLICATION_NAME_STRING;
    }

    const juce::String getApplicationVersion() override
    {
        return JUCE_APPLICATION_VERSION_STRING;
    }

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    void initialise (const juce::String& command_line) override
    {
        juce::ignoreUnused (command_line);
        mainWindow = std::make_unique<MainWindow> (getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app
        // to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& command_line) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the command_line parameter tells you what
        // the other instance's command-line arguments were.
        juce::ignoreUnused (command_line);
    }

    struct MySystrayIconComponent : public juce::SystemTrayIconComponent
    {
    public:
        void mouseUp (const juce::MouseEvent& event) override
        {
            if (onMouseUp)
            {
                onMouseUp (event);
            }
        }

        void mouseDoubleClick (const juce::MouseEvent& event) override
        {
            if (onMouseDoubleClick)
            {
                onMouseDoubleClick (event);
            }
        }

        std::function<void (const juce::MouseEvent&)> onMouseUp;
        std::function<void (const juce::MouseEvent&)> onMouseDoubleClick;
    };

    //===========================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow  : public juce::DocumentWindow,
                        private juce::Timer
    {
    public:
        explicit MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel().findColour (
                                  ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons),
              detector ("HID Detector",
                        DeviceChangeDetector::DeviceType::hid,
                        [this] (auto e, auto t) { onDeviceChange (e, t); })
        {
            setContentOwned (new MainComponent(), true);

            auto bulbImage = juce::ImageFileFormat::loadFrom (BinaryData::bulb_png,
                                                              BinaryData::bulb_pngSize);

            systrayComponent.setIconImage (bulbImage, bulbImage);

            systrayComponent.onMouseUp = [this] (const juce::MouseEvent& event)
            {
                const auto cursorPosition = juce::Desktop::getInstance().getMainMouseSource()
                                                                        .getScreenPosition();

                const auto userArea = juce::Desktop::getInstance().getDisplays()
                                                                  .getDisplayForPoint (cursorPosition.toInt(),
                                                                                       true)
                                                                  ->userArea;

                const auto newPosition = getBounds().withPosition (cursorPosition.toInt())
                                                    .constrainedWithin (userArea.reduced (10))
                                                    .getTopLeft();

                setTopLeftPosition (newPosition);
                startTimer (500);
                addToDesktop();
                toFront (true);
            };

            setCentrePosition (1000, 600);
            addToDesktop();
            toFront (true);

            removeFromDesktop();
            setVisible (true);
        }

        ~MainWindow() override
        {
            stopTimer();
        }

        void onDeviceChange (DeviceChangeDetector::DeviceEvent event, DeviceChangeDetector::DeviceType type)
        {
            bbmp::Logger::getInstance().writeLine (std::format ("Device change! Type: {} Event: {}",
                                                                enumToString (type),
                                                                enumToString (event)));

            const auto tryChangeInput = [&]
            {
                if (Monitors::getInstance().getNumMonitors() == 0)
                    return false;

                if (event == DeviceChangeDetector::DeviceEvent::arrival)
                    return Monitors::getInstance().getMonitor (0).setInput (PhysicalMonitor::Source::dp1);

                else if (event == DeviceChangeDetector::DeviceEvent::removeComplete)
                    return Monitors::getInstance().getMonitor (0).setInput (PhysicalMonitor::Source::dp2OrUsbC);

                jassertfalse;
                return true;
            };

            for (int i = 0; i < 3 && ! tryChangeInput(); ++i)
            {
                juce::Thread::sleep (500);
                Monitors::getInstance().recreatePhysicalMonitorHandles();
            }
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        void minimiseButtonPressed() override
        {
            stopTimer();
            removeFromDesktop();
        }

    private:
        void timerCallback() override
        {
            if (auto* component = getContentComponent())
                if (auto* peer = component->getPeer())
                    if (! peer->isFocused())
                        minimiseButtonPressed();
        }

        MySystrayIconComponent systrayComponent;
        DeviceChangeDetector detector;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (GuiAppApplication)
