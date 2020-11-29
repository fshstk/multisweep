# CMakeLists.txt for MultiSweep

cmake_minimum_required(VERSION 3.15)

# This file is based on the example CMakeLists.txt file for the JUCE AudioPlugin
# example. A lot of comments have been removed. For reference, see the very well
# documented CMakeLists.txt file in {JUCE_DIR}/examples/CMake/AudioPlugin.

project(MULTI_SWEEP VERSION 0.0.1)

# If you don't have JUCE installed globally (see README.md), download JUCE, put
# the JUCE folder in the project root (same directory as this file), comment out
# this line:
find_package(JUCE CONFIG REQUIRED)
# ...and uncomment this line:
# add_subdirectory(JUCE)

juce_add_plugin(MultiSweep
    COMPANY_NAME "Fabian Hummel & David Neussl (iem.at)"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE FHDN  # Fabian Hummel, David Neussl
    PLUGIN_CODE MSwp  # MultiSweep
    FORMATS VST3 Standalone  # Valid formats are: AAX Unity VST VST3 AU AUv3
    PRODUCT_NAME "MultiSweep"  # The name of the final executable.
)

# New .cpp files need to be added here:
target_sources(MultiSweep PRIVATE
    Source/PluginEditor.cpp
    Source/PluginProcessor.cpp
)

target_compile_definitions(MultiSweep PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    # As per the JUCE terms of use, the splash screen may only be disabled for
    # open-source projects or after purchasing the appropriate license.
    # Since this project is released under GPLv3, we disable it.
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(MultiSweep PRIVATE
    juce::juce_audio_utils
)
