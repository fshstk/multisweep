#pragma once
#include <cassert>
#include <utility>
#include <vector>

struct SweepSpec
{
  const double sampleRate{ 44.1e3 };
  const double duration;
  const std::pair<double, double> freqRange{ 20, 20e3 };

  bool valid() const
  {
    return (freqRange.first > 0) && (duration > 0) &&
           (freqRange.first < freqRange.second) &&
           (freqRange.second <= .5 * sampleRate);
  }
};

class LogSweep
{
public:
  using Sample = float;

public:
  LogSweep(SweepSpec spec);

  std::vector<Sample> generateSignal() const;
  std::vector<Sample> computeIR(
    const std::vector<Sample>& signalResponse) const;

private:
  std::vector<Sample> generateInverse() const;

private:
  const SweepSpec spec_;
};
