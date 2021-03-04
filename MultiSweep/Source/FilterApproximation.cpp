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
#include <cmath>
#include <nlopt.h>
#include <numeric>

std::vector<FilterParameter> FilterApproximation::calculate_filters(
  const std::vector<double>& frequencies,
  const std::vector<double>& fft_magnitudes,
  size_t max_num_filters)
{
  //
}

std::vector<double> FilterApproximation::calculate_frequency_response(
  const std::vector<FilterParameter>& filters,
  const std::vector<double>& frequencies)
{
  //
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
