#define CATCH_CONFIG_MAIN

#include "../Source/FilterApproximation.h"
#include <catch2/catch.hpp>

TEST_CASE("Check frequency response calculation")
{
  auto frequencies = std::vector<double>(110252);
  std::iota(frequencies.begin(), frequencies.end(), 0);
  std::transform(frequencies.begin(),
                 frequencies.end(),
                 frequencies.begin(),
                 [](auto x) { return x * 22050 / 110251; });

  // Taken from sweeptest.csv:
  CHECK(frequencies[0] == Approx(0.0));
  CHECK(frequencies[1] == Approx(0.199998));
  CHECK(frequencies[79390] == Approx(15877.9));
  CHECK(frequencies[98480] == Approx(19695.8));
  CHECK(frequencies[110250] == Approx(22049.8));

  const auto filters = std::vector<FilterParameter>{
    FilterParameter{ .frequency = 150, .gain_db = +5.0, .q_factor = 1.5 },
    FilterParameter{ .frequency = 900, .gain_db = -7.0, .q_factor = 1.5 },
    FilterParameter{ .frequency = 3e3, .gain_db = +6.0, .q_factor = 1.5 },
  };

  const auto magnitudes =
    FilterApproximation::calculate_frequency_response(filters, frequencies);

  // Taken from sweeptest.csv:
  // epsilon(0.05) corresponds to +/- 0.5 dB accuracy
  CHECK(magnitudes[1024] == Approx(1.47457).epsilon(0.05));
  CHECK(magnitudes[59981] == Approx(1.03161).epsilon(0.05));
  CHECK(magnitudes[79390] == Approx(1.01112).epsilon(0.05));
  CHECK(magnitudes[98480] == Approx(1.00539).epsilon(0.05));
}
