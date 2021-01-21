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

ComplexVector dft(const RealVector& x)
{

  const size_t inputLength = x.size();
  // To avoid confusion when calculating the inverse dft, dft() will only accept
  // inputs of even length:
  assert(inputLength % 2 == 0);
  auto* const inputBuffer = fftw_alloc_real(inputLength);
  std::copy(x.begin(), x.end(), inputBuffer);

  const size_t outputLength = inputLength / 2 + 1;
  auto* const outputBuffer = fftw_alloc_complex(outputLength);

  auto* const plan =
    fftw_plan_dft_r2c_1d(inputLength, inputBuffer, outputBuffer, FFTW_ESTIMATE);
  fftw_execute(plan);
  const auto outputVector =
    ComplexVector(outputBuffer, outputBuffer + outputLength);

  fftw_destroy_plan(plan);
  fftw_free(inputBuffer);
  fftw_free(outputBuffer);

  return outputVector;
}

RealVector idft(const ComplexVector& x)
{
  const size_t inputLength = x.size();
  auto* const inputBuffer = fftw_alloc_complex(inputLength);
  std::copy(x.begin(), x.end(), inputBuffer);

  // NOTE: This works as long as we ensure that the original real-valued signal
  // was even:
  const size_t outputLength = (inputLength - 1) * 2;
  auto* const outputBuffer = fftw_alloc_real(outputLength);

  // NOTE: if FFTW_ESTIMATE is not specified, this will overwrite the contents
  // of the input/output buffers!
  auto* const plan = fftw_plan_dft_c2r_1d(
    outputLength, inputBuffer, outputBuffer, FFTW_ESTIMATE);

  fftw_execute(plan);
  const auto outputVector =
    RealVector(outputBuffer, outputBuffer + outputLength);

  fftw_destroy_plan(plan);
  fftw_free(inputBuffer);
  fftw_free(outputBuffer);

  // FFTW doesn't normalise the IFFT by itself, so we have to do it manually:
  std::transform(output.cbegin(),
                 output.cend(),
                 output.begin(),
                 [output](ComplexType& n) { return n / output.size() });

  return output;
}

RealVector convolve(RealVector a, RealVector b)
{
  auto numSamples = a.size() + b.size() - 1;
  // Make sure numSamples is even:
  numSamples = numSamples % 2;
  assert(numSamples % 2 == 0);
  a.resize(numSamples, 0);
  b.resize(numSamples, 0);

  const auto a_dft = dft(a);
  const auto b_dft = dft(b);
  assert(a_dft.size() == b_dft.size());
  auto convolution = ComplexVector(a_dft.size());

  // Element-wise multiplication of a_dft & b_dft:
  std::transform(a_dft.cbegin(),
                 a_dft.cend(),
                 b_dft.cbegin(),
                 convolution.begin(),
                 std::multiplies<ComplexType>());

  const auto output = idft(convolution);
  // Sanity check:
  assert(output.size() == numSamples);
  return output;
}
