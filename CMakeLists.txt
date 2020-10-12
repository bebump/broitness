cmake_minimum_required(VERSION 3.15)

project(bebump_broitness VERSION 0.9.0)

add_subdirectory(extern/JUCE)
add_subdirectory(extern/moodycamel) # For logging

juce_add_gui_app(bebump_broitness PRODUCT_NAME "Bebump Broitness")

target_sources(
  bebump_broitness
  PRIVATE src/main.cpp
          src/main_component.cpp
          src/logging.cpp
          src/logging.h
          src/components/log_component.h
          src/components/log_component.cpp
          src/monitor.h
          src/monitor.cpp
          src/components/brightness_control.h
          src/components/brightness_control.cpp
          src/components/slider.h
          src/components/slider.cpp
          src/components/placer.h)

set_property(TARGET bebump_broitness PROPERTY CXX_STANDARD 17)

target_compile_definitions(
  bebump_broitness
  PRIVATE
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might
    # not need them.
    JUCE_WEB_BROWSER=0 # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the
    # `juce_add_gui_app` call
    JUCE_USE_CURL=0 # If you remove this, add `NEEDS_CURL TRUE` to the
    # `juce_add_gui_app` call
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:bebump_broitness,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:bebump_broitness,JUCE_VERSION>"
)

juce_add_binary_data(bebump_broitness_data SOURCES resources/bulb.png)

target_link_libraries(bebump_broitness PRIVATE bebump_broitness_data)
target_link_libraries(bebump_broitness PRIVATE concurrentqueue)
target_link_libraries(bebump_broitness PRIVATE juce::juce_gui_extra)
target_link_libraries(bebump_broitness PRIVATE Dxva2)

install(TARGETS bebump_broitness
        CONFIGURATIONS Release
        RUNTIME DESTINATION .)
