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

#include "ImpulseResponse.h"

class LogSweep : public ImpulseResponse
{
public:
  explicit LogSweep(Frequency _fs,
                    Duration _duration,
                    FreqRange _range = { 20, 20e3 })
    : ImpulseResponse(_fs, _duration, _range)
  {
    k = std::pow(range.upper / range.lower, 1 / duration);
  }
  virtual ~LogSweep() override = default;

  std::vector<float> generateSignal() const override;
  std::vector<float> generateInverse() const;
  std::vector<float> computeIR(const float signalResponse) const override;

private:
  double k;
  static constexpr auto pi = M_PI;
  std::vector<float> mutable sweep;
  std::vector<float> mutable invSweep;
};
