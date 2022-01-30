################################################################################
# FetchContent
################################################################################

include(FetchContent)

################################################################################
# JUCE
################################################################################

FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE
  GIT_TAG        6.1.4)
FetchContent_MakeAvailable(JUCE)

################################################################################
# Catch2
################################################################################

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.7)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)

################################################################################
# NLopt
################################################################################

find_package(NLopt REQUIRED)
include_directories(${NLOPT_INCLUDE_DIRS})

################################################################################
# fftw3
################################################################################

include(FindPkgConfig)
pkg_check_modules(fftw3 REQUIRED IMPORTED_TARGET fftw3)
