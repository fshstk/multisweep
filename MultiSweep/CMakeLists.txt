# ==============================================================================
#  This file is part of the IEM plug-in suite.
#  Author: Fabian Hummel, David Neussl
#  Copyright (c) 2020 - Institute of Electronic Music and Acoustics (IEM)
#  https://iem.at
#
#  The IEM plug-in suite is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  The IEM plug-in suite is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this software.  If not, see <https://www.gnu.org/licenses/>.
# ==============================================================================

juce_add_plugin(MultiSweep
    VERSION                                     0.0.1
    PLUGIN_CODE                                 MuSw
    ICON_SMALL                                  ${PROJECT_SOURCE_DIR}/resources/Standalone/IEMPluginSuiteSmall.png
    FORMATS                                     VST3 Standalone
    DESCRIPTION                                 "Multi-channel room EQ correction tool"
    VST3_CATEGORIES                             Generator Analyzer Mono Stereo Spatial Tools

    IS_SYNTH                                    FALSE
    NEEDS_MIDI_INPUT                            FALSE
    NEEDS_MIDI_OUTPUT                           FALSE
    IS_MIDI_EFFECT                              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS                 FALSE
    NEEDS_CURL                                  FALSE
    NEEDS_WEB_BROWSER                           FALSE

    # Required in Mac OS to receive audio input:
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "MultiSweep needs access to your sound devices."
)

add_library(find_peaks SHARED
    Lib/find-peaks/Util.h
    Lib/find-peaks/Util.cpp
)

juce_generate_juce_header(MultiSweep)

target_sources(MultiSweep
    PRIVATE
        # Project files:
        Source/PluginEditor.cpp
        Source/PluginProcessor.cpp
        Source/LogSweep.cpp
        Source/fft.cpp
        Source/FilterApproximation.cpp
        # IEM library:
        ../resources/Standalone/StandaloneApp.cpp
        ../resources/Standalone/IEM_AudioDeviceSelectorComponent.cpp
        ../resources/OSC/OSCParameterInterface.cpp
)

target_compile_definitions(MultiSweep
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1
        DONT_BUILD_WITH_JACK_SUPPORT=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(MultiSweep
    PRIVATE
        PkgConfig::fftw3
        NLopt::nlopt
        find_peaks
        # JUCE modules required for VST3 and Standalone:
        RobotoFont
        juce::juce_audio_plugin_client
        juce::juce_osc
        # JUCE modules required for Standalone only:
        juce::juce_audio_devices
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# ==============================================================================

juce_add_console_app(SaveAudioFiles
    NEEDS_CURL                                  FALSE
    NEEDS_WEB_BROWSER                           FALSE
)

juce_generate_juce_header(SaveAudioFiles)

target_sources(SaveAudioFiles
    PRIVATE
        Test/SaveAudioFiles.cpp
        Source/LogSweep.cpp
        Source/fft.cpp
)

target_compile_definitions(SaveAudioFiles
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(SaveAudioFiles
    PRIVATE
        PkgConfig::fftw3
        juce::juce_core
        juce::juce_audio_formats
        juce::juce_audio_processors
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

# ==============================================================================

add_executable(SweepTest)

target_sources(SweepTest
    PRIVATE
        Test/SweepTest.cpp
        Source/LogSweep.cpp
        Source/fft.cpp
)

target_link_libraries(SweepTest
    PRIVATE
        Catch2::Catch2
        PkgConfig::fftw3
)

catch_discover_tests(SweepTest)
