target_sources(${CMAKE_PROJECT_NAME} PUBLIC
  Main.cpp
  PluginEditor.cpp
  PluginProcessor.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC .)
