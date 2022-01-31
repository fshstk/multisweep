target_sources(${CMAKE_PROJECT_NAME} PUBLIC
  Main.cpp
  PluginEditor.cpp
  PluginProcessor.cpp
  LogSweep.cpp
  fft.cpp
  FilterApproximation.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC .)
