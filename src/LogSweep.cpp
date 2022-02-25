#include "LogSweep.h"
#include "FFT.h"
#include <algorithm>
#include <numeric>

namespace {

auto num_samples(const SweepSpec& spec)
{
  return static_cast<unsigned>(std::ceil(spec.duration * spec.sampleRate));
}

auto compute_k(const SweepSpec& spec)
{
  return std::pow(spec.freqRange.second / spec.freqRange.first,
                  1 / spec.duration);
}

auto time_vector(const SweepSpec& spec)
{
  std::vector<float> t(num_samples(spec));
  std::generate(std::begin(t),
                std::end(t),
                [fs = spec.sampleRate, i = 0]() mutable { return i++ / fs; });
  return t;
}

} // namespace

LogSweep::LogSweep(SweepSpec spec)
  : spec_(spec)
{
  assert(spec.valid());
}

std::vector<float> LogSweep::generateSignal() const
{
  const auto k = compute_k(spec_);

  std::vector<float> sweep;
  sweep.reserve(num_samples(spec_));
  for (const auto t : time_vector(spec_))
    sweep.emplace_back(std::sin(2 * M_PI * spec_.freqRange.first *
                                (std::pow(k, t) - 1) / std::log(k)));
  return sweep;
}

std::vector<float> LogSweep::generateInverse() const
{
  const auto time = time_vector(spec_);
  const auto k = compute_k(spec_);
  const auto sum_pow_kt = std::accumulate(
    std::cbegin(time), std::cend(time), 0.0, [=](auto total, auto t) {
      return total + std::pow(k, t);
    });

  auto invSweep = generateSignal();
  std::transform(
    std::cbegin(invSweep),
    std::cend(invSweep),
    std::cbegin(time),
    std::begin(invSweep),
    [=](auto val, auto t) { return val * 2 * (std::pow(k, t) / sum_pow_kt); });
  std::reverse(std::begin(invSweep), std::end(invSweep));
  return invSweep;
}

std::vector<float> LogSweep::computeIR(
  const std::vector<float>& signalResponse) const
{
  return convolve(signalResponse, generateInverse());
}
