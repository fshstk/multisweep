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

#include <cassert>
#include <cmath>
#include <vector>

// TODO: these should be converted to strong types at some point:
typedef double Frequency;
typedef double Duration;

struct FreqRange
{
  FreqRange(double a, double b)
    : lower(a < b ? a : b)
    , upper(a < b ? b : a)
  {}
  Frequency lower;
  Frequency upper;
};

class ImpulseResponse
{
public:
  explicit ImpulseResponse(Frequency _fs, Duration _duration, FreqRange _range)
    : fs(_fs)
    , duration(_duration)
    , range(_range)
    , numSamples(size_t(std::ceil(fs * duration)))
  {
    assert(duration > 0);
    assert(fs > 0);
    assert(range.upper < fs / 2);
  }
  virtual ~ImpulseResponse() = default;

  virtual std::vector<float> generateSignal() const = 0;
  virtual std::vector<float> computeIR(const float signalResponse) const = 0;

protected:
  inline double t(size_t i) const { return i / fs; }

protected:
  const Frequency fs;
  const Duration duration;
  const FreqRange range;
  const size_t numSamples;
};
