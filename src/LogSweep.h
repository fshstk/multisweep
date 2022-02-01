#pragma once

#include "ImpulseResponse.h"

class LogSweep : public ImpulseResponse
{
public:
  explicit LogSweep(double _fs,
                    double _duration,
                    FreqRange _range = { 20, 20e3 });
  virtual ~LogSweep() override = default;

  std::vector<float> generateSignal() const override;
  std::vector<float> generateInverse() const;
  std::vector<float> computeIR(
    const std::vector<float>& signalResponse) const override;

private:
  double k;
  static constexpr auto pi = M_PI;
  std::vector<float> mutable sweep;
  std::vector<float> mutable invSweep;
};
