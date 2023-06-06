cmake_minimum_required(VERSION 3.15)

project(bebump_broitness VERSION 0.9.2)

add_subdirectory(../JUCE-dev ${CMAKE_CURRENT_BINARY_DIR}/JUCE-dev)

juce_add_gui_app(bebump_broitness PRODUCT_NAME "Bebump Broitness")

target_sources(bebump_broitness
    PRIVATE
        src/Main.cpp
        src/MainComponent.cpp
        src/Monitor.h
        src/Monitor.cpp
        src/components/BrightnessControl.h
        src/components/BrightnessControl.cpp)

if(WIN32)
    target_sources(bebump_broitness
        PRIVATE
            src/native/MonitorWin32.cpp
            src/native/DeviceWin32.cpp)
else(APPLE)
    target_sources(bebump_broitness
        PRIVATE
            src/native/MonitorMacOS.cpp
            src/native/DeviceMacOS.cpp)
endif()

set_property(TARGET bebump_broitness PROPERTY CXX_STANDARD 17)

target_compile_definitions(bebump_broitness
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:bebump_broitness,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:bebump_broitness,JUCE_VERSION>"
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

juce_add_binary_data(bebump_broitness_data SOURCES resources/bulb.png)

target_link_libraries(bebump_broitness PRIVATE bebump_broitness_data)
target_link_libraries(bebump_broitness PRIVATE juce::juce_gui_extra)

if(WIN32)
    target_link_libraries(bebump_broitness PRIVATE Dxva2)
endif()

# ==============================================================================
# Create an installer target that will use Inno Setup 6 (must be installed) to
# create an installer that has a "Launch on Startup" option
# ==============================================================================
set(innosetup_compiler "C:/Program Files (x86)/Inno Setup 6/ISCC.exe")

add_custom_target(installer)
add_dependencies(installer bebump_broitness)
add_custom_command(TARGET installer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/installer"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:bebump_broitness>" "${CMAKE_CURRENT_BINARY_DIR}/installer/broitness.exe"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_LIST_DIR}/scripts/broitness.iss" "${CMAKE_CURRENT_BINARY_DIR}/installer"
        COMMAND ${innosetup_compiler} "${CMAKE_CURRENT_BINARY_DIR}/installer/broitness.iss"
        VERBATIM)
