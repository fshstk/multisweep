#define CATCH_CONFIG_MAIN

#include "../Source/LogSweep.h"
#include "../Source/fft.h"
#include <algorithm>
#include <catch2/catch.hpp>
#include <cmath>
#include <functional>
#include <numeric>
#include <vector>

double meanSquaredError(const std::vector<float>& a,
                        const std::vector<float>& b)
{
  assert(a.size() == b.size());
  const auto squaredError = [](auto x, auto y) { return std::pow(x - y, 2); };
  const auto sum = std::transform_reduce(
    a.cbegin(), a.cend(), b.cbegin(), 0.0, std::plus<>(), squaredError);
  return std::sqrt(sum);
}

double maxError(const std::vector<float>& a, const std::vector<float>& b)
{
  assert(a.size() == b.size());
  const auto absError = [](float x, float y) { return std::abs(x - y); };
  const auto maxValue = [](float x, float y) { return std::max(x, y); };
  const auto sum = std::transform_reduce(
    a.cbegin(), a.cend(), b.cbegin(), 0.0, maxValue, absError);
  return sum;
}

std::vector<float> simulateImpulseResponse()
{
  std::vector<float> testSystem(500, 0);
  testSystem[50] = 1.0f;
  testSystem[110] = .9f;
  std::fill(testSystem.begin() + 220, testSystem.begin() + 221, 0.2f);
  std::fill(testSystem.begin() + 300, testSystem.begin() + 305, -.15f);
  testSystem[430] = -.1f;
  testSystem[490] = .1f;
  return testSystem;
}

TEST_CASE("Check LogSweep with a generated test system", "[sweeptest]")
{
  // Set all parameters and generate forward sweep:
  float fs = 44100;
  const auto sweepObject =
    LogSweep(Frequency{ fs }, Duration{ 2 }, FreqRange{ 20, fs / 2 });
  const auto sweep = sweepObject.generateSignal();

  // Simulate an LTI system using a generated IR with arbitrary samples:
  const auto testSystem = simulateImpulseResponse();

  // Generate the simulated system response and compute the IR:
  const auto testResponse = convolve(sweep, testSystem);
  const auto measuredSystem = sweepObject.computeIR(testResponse);

  // Generate the expected IR by sandwiching testSystem in the middle of a
  // zero-initialised vector:
  std::vector<float> referenceSystem(2 * sweep.size() + testSystem.size(), 0);
  std::copy(testSystem.begin(),
            testSystem.end(),
            referenceSystem.begin() + int(sweep.size()) - 1);

  // Run tests:
  REQUIRE(measuredSystem.size() == referenceSystem.size());
  REQUIRE(meanSquaredError(measuredSystem, referenceSystem) < 0.1); // -20dB
  REQUIRE(maxError(measuredSystem, referenceSystem) < 0.01);        // -40dB
}
