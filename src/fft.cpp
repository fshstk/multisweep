#include "fft.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iterator>
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

std::vector<float> dft_lin_bins(float fs, size_t numSamples)
{
  const size_t numBins = numSamples;
  std::vector<float> bins(numBins);

  const auto bin2freq = [fs, numSamples](auto x) {
    return x * fs / numSamples;
  };

  std::iota(bins.begin(), bins.end(), 0);
  std::transform(bins.begin(), bins.end(), bins.begin(), bin2freq);
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

std::vector<float> dft_magnitude(std::vector<float> input)
{
  std::vector<double> input_double(input.cbegin(), input.cend());
  const auto output = dft_magnitude(input_double);
  return std::vector<float>(output.cbegin(), output.cend());
}

std::vector<float> dft_magnitude_db(std::vector<float> input)
{
  const auto actual_db = [](float x) { return 20.0f * std::log10(x); };
  auto mag = dft_magnitude(input);
  std::transform(mag.begin(), mag.end(), mag.begin(), actual_db);
  return mag;
}

RealVector dft_phase(RealVector input)
{
  const auto spectrum = dft(input);
  RealVector output(spectrum.size());
  const auto phase = [](auto x) { return std::arg(x); };
  std::transform(spectrum.cbegin(), spectrum.cend(), output.begin(), phase);
  return output;
}

std::vector<float> dft_log_bins(size_t num_samples = 1024,
                                float f_low = 20e0,
                                float f_high = 20e3)
{
  const auto index_to_log_bin = [=](auto x) {
    return pow(10,
               log10(f_low) +
                 (x / float(num_samples - 1) * (log10(f_high / f_low))));
  };

  auto log_bins = std::vector<float>(num_samples);
  std::iota(log_bins.begin(), log_bins.end(), 0);
  std::transform(
    log_bins.cbegin(), log_bins.cend(), log_bins.begin(), index_to_log_bin);

  return log_bins;
}

std::vector<float> dft_magnitude_with_log_bins(const std::vector<float>& input,
                                               float sampleRate,
                                               uint numbins)
{
  const auto lin_mag = dft_magnitude_db(input);
  const auto lin_bins = dft_lin_bins(sampleRate, lin_mag.size() * 2);
  const auto log_bins = dft_log_bins(numbins, 20e0, 20e3);
  auto log_mag = std::vector<float>(log_bins.size());
  const auto indices = map_log_to_lin_bins(lin_bins, log_bins);

  const auto resolve_indices = [&](auto index) { return lin_mag[index]; };
  std::transform(
    indices.cbegin(), indices.cend(), log_mag.begin(), resolve_indices);

  return log_mag;
}

std::vector<uint> map_log_to_lin_bins(const std::vector<float>& lin_bins,
                                      const std::vector<float>& log_bins)
{
  const auto index_of_first_element_greater_than = [&](auto value) {
    return std::distance(
      lin_bins.cbegin(),
      std::lower_bound(lin_bins.cbegin(), lin_bins.cend(), value));
  };

  auto indices = std::vector<uint>(log_bins.size());
  std::transform(log_bins.cbegin(),
                 log_bins.cend(),
                 indices.begin(),
                 index_of_first_element_greater_than);

  return indices;
}
