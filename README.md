![Screenshot of the Windows system tray](documentation/screenshot.png)

Minimal system tray application for Windows to control the brightness of your screen.

A single slider per monitor allows you to set the brightness by sending DDC/CI commands.

It sits minimized to your system tray, like the volume icon.

It also support automatic input switching based on USB device connection/disconnection events. I'm using a USB hub for keyboard/mouse, and when I press the switch, Broitness will automatically change the monitor's input source. 

# Dependencies

* JUCE 7.0.5

You must download it separately and specify the path to it in the `add_subdirectory` command at `CMakeLists.txt:5`.

* Inno Setup 6.2.2

If you've downloaded and installed Inno Setup you can build the `installer` target with CMake and should end up with a `broitness.exe` installer that has a *Launch at startup* option.
