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
#include <vector>

struct FilterParameter
{
  double frequency;
  double gain;
  double q_factor;
};

class FilterApproximation
{
public:
  /**
   * @brief Given an FFT with bins `frequencies` and magnitudes
   * `fft_magnitudes`, calculate the optimized filter coefficients that fit the
   * FFT with the least MSE.
   * NOTE: `fft_magnitudes` and `frequencies` must have the same size.
   *
   * @return Vector of filter coefficients. The returned vector has size
   * `max_num_filters`.
   */
  static std::vector<FilterParameter> calculate_filters(
    const std::vector<double>& frequencies,
    const std::vector<double>& fft_magnitudes,
    size_t max_num_filters = 4);

  /**
   * @brief Given a set of filter parameters, calculate the resulting
   * frequency response (i.e. the magnitude of the transfer function).
   *
   * @param filters Set of filter parameters, each with frequency, gain and Q.
   * @param frequencies Frequency bins of the resulting frequency response
   *
   * @return Vector of the same size as `frequencies`, where each element
   * corresponds to the FFT magnitude of the respective bin in `frequencies`.
   */
  static std::vector<double> calculate_frequency_response(
    const std::vector<FilterParameter>& filters,
    const std::vector<double>& frequencies);

private:
  /**
   * @brief Find the MSE between vectors a & b.
   */
  template<typename T>
  static double meanSquaredError(const std::vector<T>& a,
                                 const std::vector<T>& b);

  /**
   * @brief Return a measure of how well the given filters match the given
   * FFT. NOTE: `frequencies` and `fft_magnitudes` must have the same size.
   */
  static double cost_function(const std::vector<double>& frequencies,
                              const std::vector<double>& fft_magnitudes,
                              const std::vector<FilterParameter>& filters);
};
