target_sources(${CMAKE_PROJECT_NAME} PUBLIC
  LogSweep.cpp
  FFT.cpp
  FilterApproximation.cpp
  FreqResponseDisplay.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC .)

add_subdirectory(plugin)
