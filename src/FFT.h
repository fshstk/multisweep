#pragma once

#include <complex>
#include <fftw3.h>
#include <vector>

// Ideally, we would want to pass these two by const ref, but this would require
// a const_cast or something similar since FFTW does not take const parameters:
std::vector<std::complex<double>> dft(std::vector<double> input);
std::vector<double> idft(std::vector<std::complex<double>> input);

std::vector<double> dft_magnitude(std::vector<double> input);
std::vector<float> dft_magnitude(std::vector<float> input);
std::vector<float> dft_magnitude_db(std::vector<float> input);
std::vector<float> dft_magnitude_with_log_bins(const std::vector<float>& input,
                                               float sampleRate,
                                               size_t numbins);
std::vector<float> dft_log_bins(size_t num_samples, float f_low, float f_high);
std::vector<float> dft_lin_bins(float fs, size_t numSamples);
std::vector<double> dft_phase(std::vector<double> input);

std::vector<double> convolve(std::vector<double> a, std::vector<double> b);

// Convenience function for using floats instead of doubles:
std::vector<float> convolve(const std::vector<float>& a,
                            const std::vector<float>& b);

std::vector<size_t> map_log_to_lin_bins(const std::vector<float>& lin_bins,
                                        const std::vector<float>& log_bins);
