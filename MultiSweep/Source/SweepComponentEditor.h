/*
 ==============================================================================
 This file is part of the IEM plug-in suite.
 Authors: Fabian Hummel, David Neussl
 Copyright (c) 2020 - Institute of Electronic Music and Acoustics (IEM)
 https://iem.at

 The IEM plug-in suite is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 The IEM plug-in suite is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this software.  If not, see <https://www.gnu.org/licenses/>.
 ==============================================================================
 */

#pragma once
#include "SweepComponentProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

/*

SweepComponent has 4 waveform displays:
1. raw recorded sweep response (live maybe?)
2. calculated IR
3. calculated freq response
4. calculated phase

separate class: audio thumbnail, takes in buffer/vector by reference,
                has a callback to redraw

*/

// class SweepComponentEditor : public juce::AudioProcessorEditor
// {
// public:
//   SweepComponentEditor(SweepComponentProcessor& sweepProcessor)
//     : AudioProcessorEditor(sweepProcessor)
//   {}
// };
