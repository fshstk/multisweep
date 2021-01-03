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
  , sampleRate(0)
{}

Sweep::Sweep(double _sampleRate)
  : audioContext(nullptr)
  , sampleRate(_sampleRate)
{}

std::vector<double> Sweep::linear(float duration, FreqRange range) const
{
  return this->generateSweep(duration, range, Linear, false);
}

std::vector<double> Sweep::inverseLinear(float duration, FreqRange range) const
{
  return this->generateSweep(duration, range, Linear, true);
}

std::vector<double> Sweep::exponential(float duration, FreqRange range) const
{
  return this->generateSweep(duration, range, Exponential, false);
}

std::vector<double> Sweep::inverseExponential(float duration,
                                              FreqRange range) const
{
  return this->generateSweep(duration, range, Exponential, true);
}

std::vector<double> Sweep::generateSweep(float duration,
                                         FreqRange range,
                                         SweepType type,
                                         bool inverse) const
{
  std::vector<double> sweep;
  const auto fs = getSampleRate();
  const auto numSamples = std::ceil(duration * fs);

  for (auto i = 0; i < numSamples; ++i) {
    const auto t = i / fs;
    double value;

    if (type == Linear) {
      const auto k = (range.upper - range.lower) * (1 / duration);
      value = std::sin(2 * pi * (0.5 * k * std::pow(t, 2) + range.lower * t));

      if (inverse)
        value *= (1 / fs);
    }

    else if (type == Exponential) {
      const auto k = std::pow(range.upper - range.lower, 1 / duration);
      value =
        std::sin(2 * pi * range.lower * (std::pow(k, t) - 1) / std::log(k));

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

double Sweep::getSampleRate() const
{
  if (audioContext != nullptr)
    return audioContext->getSampleRate();
  else
    return sampleRate;
}
