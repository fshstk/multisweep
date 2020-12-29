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
)

juce_generate_juce_header(MultiSweep)

target_sources(MultiSweep
    PRIVATE
        Source/PluginEditor.cpp
        Source/PluginProcessor.cpp
        ../resources/OSC/OSCParameterInterface.cpp
        ../resources/OSC/OSCStatus.cpp
)

target_compile_definitions(MultiSweep
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0 # FIXME: this should be off by default!
        JUCE_DISPLAY_SPLASH_SCREEEN=0
        # JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1 # FIXME: this doesn't work
)

target_link_libraries(MultiSweep
    PRIVATE
        # Required for VST3 and Standalone:
        RobotoFont
        juce::juce_audio_plugin_client
        juce::juce_osc
        # Required for Standalone only:
        juce::juce_audio_devices
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        # FIXME: no frameworks should need to be inclded explicitly!
        # # Required for VST3 and Standalone:
        # "-framework CoreImage"
        # "-framework AppKit"
        # "-framework IOKit"
        # "-framework Accelerate"
        # # Required for Standalone only:
        # "-framework CoreMidi"
        # "-framework CoreAudio"
        # "-framework AudioToolbox"
        # "-framework CoreAudioKit"
)
