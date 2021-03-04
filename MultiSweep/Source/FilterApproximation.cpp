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

#include "FilterApproximation.h"
#include <Util.h>
#include <algorithm>
#include <cmath>
#include <complex>
#include <functional>
#include <nlopt.h>
#include <numeric>
#include <cassert>
#include <complex>

std::vector<FilterParameter> FilterApproximation::calculate_filters(
  const std::vector<double>& frequencies,
  const std::vector<double>& fft_magnitudes,
  size_t max_num_filters)
{
  // determine lower frequency bound
  int iF_lower = 0;
  while(frequencies[iF_lower] < 20)
  {
    iF_lower += 1;
  }
  iF_lower -= 1;

  // determine lower frequency bound
  int iF_upper = frequencies.size();
  while(frequencies[iF_upper] > 20000)
  {
    iF_upper -= 1;
  }
  iF_upper += 1;

  std::vector<double> vF = std::vector<double>(frequencies[iF_lower], frequencies[iF_upper]);
  std::vector<double> vMag = std::vector<double>(fft_magnitudes[iF_lower], fft_magnitudes[iF_upper]);
  std::vector<std::complex<double>> vS(vF.size());
  for(int iS = 0; iS < vF.size(); iS++)
  {
    const std::complex<double> tmp(0, vF[iS] * 2 * M_PI);
    vS.push_back(tmp);
  }

  // initial guesses
  std::vector<int> iPeaks;
  std::vector<float> vMagTmp(vMag.begin(), vMag.end());
  Peaks::findPeaks(vMagTmp, iPeaks);
  int num_filters = iPeaks.size();
  std::vector<double> vFc(num_filters);
  std::vector<double> vAc(num_filters);
  std::vector<double> vQc(num_filters);
  std::vector<double> vB0(3 * num_filters);
  for(int i = 0; i < vFc.size(); i++)
  {
    vFc[i] = vF[iPeaks[i]];
    vAc[i] = vMag[iPeaks[i]];
    vQc[i] = 1;
    vB0[i * num_filters] = vFc[i];
    vB0[(i + 1) * num_filters] = vAc[i];
    vB0[(i + 2) * num_filters] = vQc[i];
  }
  return {};
}

std::vector<double> FilterApproximation::calculate_frequency_response(
  const std::vector<FilterParameter>& filters,
  const std::vector<double>& frequencies)
{
  auto total_filter_response = std::vector<double>(frequencies.size());

  for (const auto& filter : filters) {
    const auto transfer_function_mag = [filter](auto frequency) {
      const auto omega = 2 * M_PI * frequency;
      const auto s = 1i * omega;

      const auto w0 = 2 * M_PI * filter.frequency;
      const auto Q = filter.q_factor;
      const auto A = filter.gain;

      const auto s_squared = std::pow(s, 2);
      const auto w0_squared = std::pow(w0, 2);

      // Transfer function for bell filter:
      //        s^2 + (A * w0 / Q) s + w0^2
      // H(s) = ---------------------------
      //        s^2 +     (w0 / Q) s + w0^2

      const auto H = (s_squared + (A * w0 / Q) * s + w0_squared) /
                     (s_squared + (w0 / Q) * s + w0_squared);
      return std::abs(H);
    };

    // Calculate the response for the current loop iteration:
    auto single_filter_response = std::vector<double>(frequencies.size());
    std::transform(frequencies.cbegin(),
                   frequencies.cend(),
                   single_filter_response.begin(),
                   transfer_function_mag);

    // Multiply the response with the total response:
    std::transform(single_filter_response.cbegin(),
                   single_filter_response.cend(),
                   total_filter_response.cbegin(),
                   total_filter_response.begin(),
                   std::multiplies<>());
  }

  return total_filter_response;
}

template<typename T>
double FilterApproximation::meanSquaredError(const std::vector<T>& a,
                                             const std::vector<T>& b)
{
  assert(a.size() == b.size());
  const auto squaredError = [](auto x, auto y) { return std::pow(x - y, 2); };
  const auto sum = std::transform_reduce(
    a.cbegin(), a.cend(), b.cbegin(), 0.0, std::plus<>(), squaredError);
  return std::sqrt(sum);
}

double FilterApproximation::cost_function(
  const std::vector<double>& frequencies,
  const std::vector<double>& fft_magnitudes,
  const std::vector<FilterParameter>& filters)
{
  assert(frequencies.size() == fft_magnitudes.size());

  const auto expected_response = fft_magnitudes;
  const auto calculated_response =
    calculate_frequency_response(filters, frequencies);

  return meanSquaredError(calculated_response, expected_response);
}
