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
                                          bool inverse,
                                          float durationInSeconds,
                                          float startFreq = 20,
                                          float endFreq = 20e3)
{
  std::vector<double> sweep;
  const auto fs = getSampleRate();
  const auto numSamples = std::ceil(durationInSeconds * fs);

  for (auto i = 0; i < numSamples; ++i) {
    const auto t = i / fs;
    double value;

    if (type == Linear) {
      const auto k = (endFreq - startFreq) * (1 / durationInSeconds);
      value = std::sin(2 * pi * (0.5 * k * std::pow(t, 2) + startFreq * t));

      if (inverse)
        value *= (1 / fs);
    }

    else if (type == Exponential) {
      const auto k = std::pow(endFreq - startFreq, 1 / durationInSeconds);
      value = std::sin(2 * pi * startFreq * (std::pow(k, t) - 1) / std::log(k));

      if (inverse) {
        value *= 2 * std::pow(k, t);
        // Factor in next line is equivalent to sum(k^t) for all t in range:
        value *= (1 - std::pow(k, numSamples / fs)) / (1 - std::pow(k, 1 / fs));
      }
    }

    sweep.push_back(value);
  }

  if (inverse)
    std::reverse(sweep.begin(), sweep.end());

  return sweep;
}
