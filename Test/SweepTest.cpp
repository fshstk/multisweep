#include "LogSweep.h"
#include "fft.h"
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

// TODO: Perform this test but with multiple sample frequencies, sweep
// durations, and simulated IRs.
TEST_CASE("Check LogSweep with a generated test system", "[sweeptest]")
{
  // Set all parameters and generate forward sweep:
  float fs = 44100;
  const auto sweepObject =
    LogSweep(Frequency{ fs }, Duration{ 2.5 }, FreqRange{ 1, fs / 2 });
  const auto sweep = sweepObject.generateSignal();

  // Simulate an LTI system using a generated IR with arbitrary samples:
  const auto testSystem = simulateImpulseResponse();

  // Generate the simulated system response and compute the IR:
  const auto testResponse = convolve(sweep, testSystem);
  const auto measuredSystem = sweepObject.computeIR(testResponse);

  // Generate the expected IR by sandwiching testSystem in the middle of a
  // zero-initialised vector:
  const size_t referenceLength =
    2 * (sweep.size() - sweep.size() % 2) + testSystem.size();
  std::vector<float> referenceSystem(referenceLength, 0);
  std::copy(testSystem.begin(),
            testSystem.end(),
            referenceSystem.begin() + int(sweep.size()) - 1);

  // Run tests:
  REQUIRE(measuredSystem.size() == referenceSystem.size());

  // NOTE: These values were determined more or less by trial and error. A
  // successful implementation of the sweep has values that are just under the
  // limits specified here:
  CHECK(meanSquaredError(measuredSystem, referenceSystem) < 0.1); // -20dB
  CHECK(maxError(measuredSystem, referenceSystem) < 0.01);        // -40dB
}

TEST_CASE("Check lin bins (even N)")
{
  const auto bins = dft_lin_bins(44100, 1024);
  REQUIRE(bins.size() == 1024);

  // Reference values generated using Python:
  // bins = np.linspace(0, 44100, 1024, endpoint=False)
  REQUIRE(bins[0] == Approx(0.0));
  REQUIRE(bins[512] == Approx(22050.0));
  REQUIRE(bins[511] == Approx(22006.93359375));
  REQUIRE(bins[200] == Approx(8613.28125));
  REQUIRE(bins[348] == Approx(14987.109375));
}

TEST_CASE("Check lin bins (odd N)")
{
  const auto bins = dft_lin_bins(44100, 999);
  REQUIRE(bins.size() == 999);

  // Reference values generated using Python:
  // bins = np.linspace(0, 44100, 999, endpoint=False)
  REQUIRE(bins[0] == Approx(0.0));
  REQUIRE(bins[499] == Approx(22027.92792792793));
  REQUIRE(bins[111] == Approx(4900.0));
  REQUIRE(bins[200] == Approx(8828.828828828828));
  REQUIRE(bins[348] == Approx(15362.162162162162));
}

TEST_CASE("Check log bins")
{
  const auto bins = dft_log_bins(1024, 20e0, 20e3);
  REQUIRE(bins.size() == 1024);

  // Reference values generated using Python:
  // bins = np.logspace(np.log10(20), np.log10(20e3), 1024)
  REQUIRE(bins[0] == Approx(20e0f));
  REQUIRE(bins[1023] == Approx(20e3f));
  REQUIRE(bins[30] == Approx(24.491000955696432f));
  REQUIRE(bins[348] == Approx(209.6804089192401f));
}

TEST_CASE("Check mapping of log bins to lin bin indices")
{
  // Python code:
  // def lower_bound(array, value):
  //     return [x for x, y in enumerate(array) if y >= value][0]
  // lin_bins = np.linspace(0, 44100, 88200, endpoint=False)
  // log_bins = np.logspace(np.log10(20), np.log10(20e3), 1024)
  // log2lin = [lower_bound(lin_bins, x) for x in log_bins]

  const auto lin_bins = dft_lin_bins(44100, 88200);
  const auto log_bins = dft_log_bins(1024, 20e0, 20e3);
  const auto indices = map_log_to_lin_bins(lin_bins, log_bins);

  REQUIRE(indices.size() == log_bins.size());
  REQUIRE(indices[0] == 41);
  REQUIRE(indices[100] == 79);
  REQUIRE(indices[246] == 211);
  REQUIRE(indices[1023] == 40001);
  REQUIRE(indices[30] == 49);
  REQUIRE(indices[348] == 420);
}
