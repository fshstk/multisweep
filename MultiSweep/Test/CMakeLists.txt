juce_add_console_app(SaveAudioFiles
    NEEDS_CURL                                  FALSE
    NEEDS_WEB_BROWSER                           FALSE
)

juce_generate_juce_header(SaveAudioFiles)

target_sources(SaveAudioFiles
    PRIVATE
        SaveAudioFiles.cpp
        ../Source/LogSweep.cpp
        ../Source/fft.cpp
)

target_compile_definitions(SaveAudioFiles
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(SaveAudioFiles
    PRIVATE
        fftw3
        juce::juce_core
        juce::juce_audio_formats
        juce::juce_audio_processors
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

# ==============================================================================

add_executable(SweepTest)
add_executable(FilterParameterTest)

target_sources(SweepTest
    PRIVATE
        SweepTest.cpp
        ../Source/LogSweep.cpp
        ../Source/fft.cpp
)

target_link_libraries(SweepTest
    PRIVATE
        Catch2::Catch2
        fftw3
)

target_sources(FilterParameterTest
    PRIVATE
        FilterParameterTest.cpp
        ../Source/FilterApproximation.cpp
)

target_link_libraries(FilterParameterTest
    PRIVATE
        Catch2::Catch2
        find_peaks
)

catch_discover_tests(SweepTest)
catch_discover_tests(FilterParameterTest)
