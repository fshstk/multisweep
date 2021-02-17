# ==============================================================================
#  This file is part of the IEM plug-in suite.
#  Author: Fabian Hummel
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

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(IEMPluginSuite
    VERSION     1.11.1
    LANGUAGES   C CXX
)

find_package(JUCE REQUIRED)
find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)
include(FindPkgConfig)

pkg_check_modules(fftw3 REQUIRED IMPORTED_TARGET fftw3)

set_directory_properties(PROPERTIES
    JUCE_COMPANY_NAME               "IEM"
    JUCE_COMPANY_WEBSITE            "iem.at"
    JUCE_COMPANY_EMAIL              "office@iem.at"
    JUCE_COMPANY_COPYRIGHT          "(c) 2020 Institute of Electronic Music, Graz University of the Arts"
    JUCE_PLUGIN_MANUFACTURER_CODE   "IEMA"
    JUCE_COPY_PLUGIN_AFTER_BUILD    TRUE
)

include_directories(resources)

juce_add_binary_data(RobotoFont SOURCES
        resources/lookAndFeel/Roboto-Bold.ttf
        resources/lookAndFeel/Roboto-Light.ttf
        resources/lookAndFeel/Roboto-Medium.ttf
        resources/lookAndFeel/Roboto-Regular.ttf
)

add_subdirectory(MultiSweep)
