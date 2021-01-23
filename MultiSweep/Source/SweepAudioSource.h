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
#include "LogSweep.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

class SweepAudioSource : public juce::AudioTransportSource
{
public:
  SweepAudioSource();
  ~SweepAudioSource() = default;

  void prepareToPlay(int samplesPerBlock, double sampleRate) override;
  void playFromStart();

private:
  juce::AudioSampleBuffer makeAudioBuffer(
    const std::vector<float>& vector) const;

private:
  const double duration = 2;
  const double lowerFreq = 20;
  const double upperFreq = 20e3;

  double fs;
  std::unique_ptr<juce::MemoryAudioSource> audioSource;
};
