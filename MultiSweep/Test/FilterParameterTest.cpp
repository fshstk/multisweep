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
  REQUIRE(frequencies[0] == Approx(0.0));
  REQUIRE(frequencies[1] == Approx(0.199998));
  REQUIRE(frequencies[79390] == Approx(15877.9));
  REQUIRE(frequencies[98480] == Approx(19695.8));
  REQUIRE(frequencies[110250] == Approx(22049.8));

  const auto filters = std::vector<FilterParameter>{
    FilterParameter{ .frequency = 150, .gain_db = +5.0, .q_factor = 1.0 },
    FilterParameter{ .frequency = 900, .gain_db = -7.0, .q_factor = 1.0 },
    FilterParameter{ .frequency = 3e3, .gain_db = +6.0, .q_factor = 1.0 },
  };

  const auto magnitudes =
    FilterApproximation::calculate_frequency_response(filters, frequencies);

  // Taken from sweeptest.csv:
  REQUIRE(magnitudes[79390] == Approx(1.01112));
  REQUIRE(magnitudes[98480] == Approx(1.00539));
  REQUIRE(magnitudes[110250] == Approx(0.4324));
}
