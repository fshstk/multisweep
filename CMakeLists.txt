cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_OSX_ARCHITECTURES x86_64)

project(MultiSweep
    VERSION     0.2.0
    LANGUAGES   CXX)

include(Dependencies.cmake)

enable_testing()

include_directories(resources)

add_subdirectory(font)

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

target_compile_definitions(${CMAKE_PROJECT_NAME}
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_MODAL_LOOPS_PERMITTED=1
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        fftw3
        nlopt
        find_peaks

        RobotoFont

        juce::juce_audio_utils

        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
