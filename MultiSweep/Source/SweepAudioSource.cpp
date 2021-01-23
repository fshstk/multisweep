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

#include "SweepAudioSource.h"

SweepAudioSource::SweepAudioSource() {}

void SweepAudioSource::prepareToPlay(int samplesPerBlock, double sampleRate)

{
  fs = sampleRate;
  AudioTransportSource::prepareToPlay(samplesPerBlock, sampleRate);
}

void SweepAudioSource::playFromStart()
{
  const auto sweep = LogSweep(fs, duration, { lowerFreq, upperFreq });
  auto buffer = makeAudioBuffer(sweep.generateSignal());

  setSource(nullptr); // need this to avoid segfault
  audioSource = std::make_unique<juce::MemoryAudioSource>(buffer, true);
  setSource(audioSource.get());

  start();
}

juce::AudioSampleBuffer SweepAudioSource::makeAudioBuffer(
  const std::vector<float>& vector) const
{
  juce::AudioSampleBuffer buffer(1, int(vector.size()));
  for (size_t i = 0; i < vector.size(); ++i)
    buffer.setSample(0, int(i), vector[i]);
  return buffer;
}
