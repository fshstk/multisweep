add_library(find_peaks
    SHARED
        Util.cpp
)

target_include_directories(find_peaks
    INTERFACE
        .
)
