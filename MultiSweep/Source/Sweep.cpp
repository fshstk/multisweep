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

#include "Sweep.h"

Sweep::Sweep(juce::AudioProcessor* _audioContext)
  : audioContext(_audioContext)
{}

const std::vector<double> Sweep::generate(SweepType type,
                                          float durationInSeconds,
                                          float startFreq,
                                          float endFreq)
{
  const auto fs = getSampleRate();
  const auto numSamples = size_t(durationInSeconds * fs);

  std::vector<double> sweep;
  sweep.reserve(numSamples);

  for (size_t i = 0; i < numSamples; ++i) {
    const auto t = i / fs;

    if (type == Linear || type == InverseLinear) {
      const auto k = (endFreq - startFreq) * (1 / durationInSeconds);
      const auto value = sin(2 * pi * (0.5 * k * pow(t, 2) + startFreq * t));
      sweep.push_back(value);
    } else {
      const auto k = pow(endFreq - startFreq, 1 / durationInSeconds);
      const auto value = sin(2 * pi * startFreq * (pow(k, t) - 1) / log(k));
      sweep.push_back(value);
    }
  }

  return sweep;
}
