![Screenshot of the Windows system tray](documentation/screenshot.png)

Minimal system tray application for Windows to control the brightness of your
screen.

A single slider per monitor allows you to set the brightness by sending DDC/CI
commands.

It sits minimized to your system tray, like the volume icon.


# Building

You need CMake 3.15+ and Visual Studio 2017+ installed on your system. If you
also have Python 3.6+ you can just run `scripts/run_test_and_build.py` which
will build everything in a temp directory and install the build artifacts to
the `install` directory of the repository.

If you also have InnoSetup on your computer you can run the `.iss` installer
generator script from the `install` directory after the build is finished, 
which will wrap up the executable to an installer that can automatically launch
the program after Windows startup.
