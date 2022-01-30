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

FetchContent_Declare(
  NLopt
  GIT_REPOSITORY https://github.com/stevengj/nlopt.git
  GIT_TAG        v2.7.1)
FetchContent_MakeAvailable(NLopt)

include_directories(${nlopt_SOURCE_DIR}/src/api)

################################################################################
# fftw3
################################################################################

FetchContent_Declare(
  fftw3
  URL       http://fftw.org/fftw-3.3.10.tar.gz
  URL_HASH  MD5=8ccbf6a5ea78a16dbc3e1306e234cc5c)
FetchContent_MakeAvailable(fftw3)

include_directories(${fftw3_SOURCE_DIR}/api)

################################################################################
# find-peaks
################################################################################

FetchContent_Declare(
  find_peaks
  GIT_REPOSITORY https://github.com/claydergc/find-peaks.git
  GIT_TAG        4b9c56671e43554a26bbba28ec7420458b3eaaea)
FetchContent_MakeAvailable(find_peaks)

add_library(find_peaks SHARED ${find_peaks_SOURCE_DIR}/Util.cpp)
target_include_directories(find_peaks INTERFACE ${find_peaks_SOURCE_DIR})