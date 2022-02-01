#pragma once

#include <cassert>
#include <cmath>
#include <vector>

struct FreqRange
{
  FreqRange(double a, double b)
    : lower(a < b ? a : b)
    , upper(a < b ? b : a)
  {}
  double lower;
  double upper;
};

class ImpulseResponse
{
public:
  explicit ImpulseResponse(double _fs, double _duration, FreqRange _range)
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
  const double fs;
  const double duration;
  const FreqRange range;
  const size_t numSamples;
};
