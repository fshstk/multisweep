# MultiSweep -- multichannel room correction tool

## Install

### Dependencies

You need to have CMake installed before continuing. You can either grab it
directly off the [website](https://cmake.org/) or use the package manager of
your choice.

You also need the JUCE framework. There are two methods for including JUCE in
this project:

### Add JUCE locally (easier if you are building just this project)

1. Download JUCE from the [website](https://juce.com/).
2. Place the JUCE directory inside the MultiSweep project root folder.
3. Open CMakeLists.txt and find the following lines:

```cmake
# If you don't have JUCE installed globally (see README.md), download JUCE, put
# the JUCE folder in the project root (same directory as this file), comment out
# this line:
find_package(JUCE CONFIG REQUIRED)
# ...and uncomment this line:
# add_subdirectory(JUCE)
```

4. Follow the instructions in the CMakeLists file.

### Install JUCE globally (more convenient if you regularly work with JUCE)

1. Download JUCE from the [website](https://juce.com/).
2. `cd` into the JUCE directory.
3. Execute the following commands in your shell:
   TODO: install

```bash
mkdir build
cd build
cmake .
cmake --build .
```

JUCE is now installed globally for your machine and can be used in any future
projects.

### Build

TODO: build

### TODO: VS Code (?)
