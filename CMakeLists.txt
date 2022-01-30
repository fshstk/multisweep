cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(IEMPluginSuite
    VERSION     1.11.1
    LANGUAGES   C CXX
)

include(Dependencies.cmake)

enable_testing()

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
