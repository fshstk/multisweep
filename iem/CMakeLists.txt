add_subdirectory(font)

add_library(iem)

target_sources(iem PUBLIC IEM_LaF.cpp)
target_include_directories(iem PUBLIC .)
target_link_libraries(iem PUBLIC juce::juce_gui_basics RobotoFont)
