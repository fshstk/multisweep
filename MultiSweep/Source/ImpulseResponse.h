#pragma once

#include <cassert>
#include <cmath>
#include <vector>

// TODO: these should be converted to strong types at some point:
typedef double Frequency;
typedef double Duration;

struct FreqRange
{
  FreqRange(double a, double b)
    : lower(a < b ? a : b)
    , upper(a < b ? b : a)
  {}
  Frequency lower;
  Frequency upper;
};

class ImpulseResponse
{
public:
  explicit ImpulseResponse(Frequency _fs, Duration _duration, FreqRange _range)
    : fs(_fs)
    , duration(_duration)
    , range(_range)
    , numSamples(size_t(std::ceil(fs * duration)))
  {
    assert(duration > 0);
    assert(fs > 0);
    assert(range.upper <= fs / 2);
  }
  virtual ~ImpulseResponse() = default;

  virtual std::vector<float> generateSignal() const = 0;
  virtual std::vector<float> computeIR(
    const std::vector<float>& signalResponse) const = 0;

protected:
  inline double t(size_t i) const { return i / fs; }

protected:
  const Frequency fs;
  const Duration duration;
  const FreqRange range;
  const size_t numSamples;
};
