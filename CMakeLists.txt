cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(MultiSweep
    VERSION     0.2.0
    LANGUAGES   CXX)

include(Dependencies.cmake)

enable_testing()

include_directories(resources)

juce_add_binary_data(RobotoFont SOURCES
        resources/lookAndFeel/Roboto-Bold.ttf
        resources/lookAndFeel/Roboto-Light.ttf
        resources/lookAndFeel/Roboto-Medium.ttf
        resources/lookAndFeel/Roboto-Regular.ttf
)

juce_add_plugin(${CMAKE_PROJECT_NAME}
    PLUGIN_CODE                                 MuSw
    FORMATS                                     AU VST3 Standalone
    DESCRIPTION                                 "Multi-channel room EQ correction tool"
    COMPANY_NAME                                "Fabian Hummel & David Neussl"
    PLUGIN_MANUFACTURER_CODE                    FHDN

    IS_SYNTH                                    FALSE
    NEEDS_MIDI_INPUT                            FALSE
    COPY_PLUGIN_AFTER_BUILD                     TRUE

    # Required in Mac OS to receive audio input:
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "MultiSweep needs access to your sound devices."
)

add_subdirectory(src)
add_subdirectory(test)

juce_generate_juce_header(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        # IEM library:
        ../resources/Standalone/StandaloneApp.cpp
        ../resources/Standalone/IEM_AudioDeviceSelectorComponent.cpp
        ../resources/OSC/OSCParameterInterface.cpp
)

target_compile_definitions(${CMAKE_PROJECT_NAME}
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1
        DONT_BUILD_WITH_JACK_SUPPORT=1
        JUCE_MODAL_LOOPS_PERMITTED=1
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        fftw3
        nlopt
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
