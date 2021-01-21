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

#include "fft.h"
#include <algorithm>
#include <cassert>
#include <fftw3.h>
#include <functional>

ComplexVector complexExponential(double k, int numSamples)
{
  //
}

ComplexVector dft(const RealVector& x)
{
  //
}

RealVector idft(const ComplexVector& x)
{
  //
}

RealVector convolve(RealVector a, RealVector b)
{
  const auto numSamples = a.size() + b.size() - 1;
  a.resize(numSamples, 0);
  b.resize(numSamples, 0);
  auto convolution = ComplexVector(numSamples);

  const auto a_dft = dft(a);
  const auto b_dft = dft(b);

  // Element-wise multiplication of a_dft & b_dft:
  std::transform(a_dft.cbegin(),
                 a_dft.cend(),
                 b_dft.cbegin(),
                 convolution.begin(),
                 std::multiplies<std::complex<double>>());

  return idft(convolution);
}
