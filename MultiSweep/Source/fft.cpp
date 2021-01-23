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
#include <functional>
#include <numeric>

ComplexVector dft(RealVector input)
{
  // To avoid confusion when calculating the inverse dft, dft() needs an input
  // of even length:
  input.resize(input.size() + input.size() % 2, 0);
  ComplexVector output(input.size() / 2 + 1);

  // NOTE: if FFTW_ESTIMATE is not specified, this will overwrite the contents
  // of the input/output buffers!
  auto* const plan =
    fftw_plan_dft_r2c_1d(int(input.size()),
                         input.data(),
                         reinterpret_cast<fftw_complex*>(output.data()),
                         FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);

  return output;
}

RealVector idft(ComplexVector input)
{
  // NOTE: This works as long as we ensure that the original real-valued signal
  // was even:
  RealVector output((input.size() - 1) * 2);

  // NOTE: if FFTW_ESTIMATE is not specified, this will overwrite the contents
  // of the input/output buffers!
  auto* const plan =
    fftw_plan_dft_c2r_1d(int(output.size()),
                         reinterpret_cast<fftw_complex*>(input.data()),
                         output.data(),
                         FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);

  // FFTW doesn't normalise the IFFT by itself, so we have to do it manually:
  std::for_each(output.begin(), output.end(), [output](RealType& n) {
    n /= output.size();
  });

  return output;
}

RealVector convolve(RealVector a, RealVector b)
{
  auto outputSize = a.size() + b.size() - 1;
  // Make sure outputSize is even:
  outputSize += outputSize % 2;
  assert(outputSize % 2 == 0);
  a.resize(outputSize, 0);
  b.resize(outputSize, 0);

  const auto a_dft = dft(a);
  const auto b_dft = dft(b);
  assert(a_dft.size() == b_dft.size());
  auto convolution = ComplexVector(a_dft.size());

  // Element-wise multiplication of a_dft & b_dft:
  std::transform(a_dft.cbegin(),
                 a_dft.cend(),
                 b_dft.cbegin(),
                 convolution.begin(),
                 std::multiplies<>());

  const auto output = idft(convolution);
  // Sanity check:
  assert(output.size() == outputSize);
  return output;
}

std::vector<float> convolve(const std::vector<float>& a,
                            const std::vector<float>& b)
{
  std::vector<double> a_double(a.cbegin(), a.cend());
  std::vector<double> b_double(b.cbegin(), b.cend());
  const auto output = convolve(a_double, b_double);
  return std::vector<float>(output.cbegin(), output.cend());
}

RealVector dft_bins(float fs, size_t numSamples)
{
  const size_t numBins = size_t(std::ceil(numSamples / 2)) + 1;
  RealVector bins(numBins);

  const auto bin2freq = [fs, numSamples](auto x) {
    return x * fs / numSamples;
  };

  std::iota(bins.begin(), bins.end(), 0);
  std::for_each(bins.begin(), bins.end(), bin2freq);
  return bins;
}

RealVector dft_magnitude(RealVector input)
{
  const auto spectrum = dft(input);
  RealVector output(spectrum.size());
  const auto magnitude = [](auto x) { return std::abs(x); };
  std::transform(spectrum.cbegin(), spectrum.cend(), output.begin(), magnitude);
  return output;
}

RealVector dft_phase(RealVector input)
{
  const auto spectrum = dft(input);
  RealVector output(spectrum.size());
  const auto phase = [](auto x) { return std::arg(x); };
  std::transform(spectrum.cbegin(), spectrum.cend(), output.begin(), phase);
  return output;
}
