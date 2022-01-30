cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
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

add_subdirectory(MultiSweep)
