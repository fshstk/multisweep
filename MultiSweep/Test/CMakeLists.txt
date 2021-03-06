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
        PkgConfig::fftw3
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
