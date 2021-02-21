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

#include <complex>
#include <fftw3.h>
#include <vector>

// These MUST be "double" and "fftw_complex" for fftw to work!
// (std::complex<double> may also work but is not guaranteed to.)
typedef double RealType;
typedef std::complex<RealType> ComplexType;
typedef std::vector<RealType> RealVector;
typedef std::vector<ComplexType> ComplexVector;

// Ideally, we would want to pass these two by const ref, but this would require
// a const_cast or something similar since FFTW does not take const parameters:
ComplexVector dft(RealVector input);
RealVector idft(ComplexVector input);

RealVector dft_lin_bins(float fs, size_t numSamples);
RealVector dft_magnitude(RealVector input);
std::vector<float> dft_magnitude(std::vector<float> input);
std::vector<float> dft_log_bins(size_t num_samples, float f_low, float f_high);
std::vector<float> dft_lin_bins(float fs, size_t numSamples);
RealVector dft_phase(RealVector input);

RealVector convolve(RealVector a, RealVector b);

// Convenience function for using floats instead of doubles:
std::vector<float> convolve(const std::vector<float>& a,
                            const std::vector<float>& b);
