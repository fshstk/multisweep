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

#include "LogSweep.h"
#include "fft.h"

LogSweep::LogSweep(Frequency _fs, Duration _duration, FreqRange _range)
  : ImpulseResponse(_fs, _duration, _range)
{
  k = std::pow(range.upper / range.lower, 1 / duration);
}

std::vector<float> LogSweep::generateSignal() const
{
  if (!sweep.empty())
    return sweep;

  sweep = std::vector<float>(numSamples);

  for (size_t i = 0; i < numSamples; ++i)
    sweep[i] = float(
      std::sin(2 * pi * range.lower * (std::pow(k, t(i)) - 1) / std::log(k)));

  return sweep;
}

std::vector<float> LogSweep::generateInverse() const
{
  if (!invSweep.empty())
    return invSweep;

  invSweep = generateSignal();

  // Factor is equivalent to sum(k^t) for all t in range:
  const auto factor =
    (1 - std::pow(k, 1 / fs)) / (1 - std::pow(k, numSamples / fs));

  // Amplitude scaling:
  for (size_t i = 0; i < numSamples; ++i)
    invSweep[i] *= float(2 * factor * std::pow(k, t(i)));

  std::reverse(invSweep.begin(), invSweep.end());
  return invSweep;
}

std::vector<float> LogSweep::computeIR(
  const std::vector<float>& signalResponse) const
{
  return convolve(signalResponse, generateInverse());
}
