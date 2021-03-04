#define CATCH_CONFIG_MAIN

#include "../Source/FilterApproximation.h"
#include <catch2/catch.hpp>

TEST_CASE("Check frequency response calculation")
{
  const auto frequencies = std::vector<double>{};
  const auto filters = std::vector<FilterParameter>{};

  const auto reference_response = std::vector<double>{};
  const auto calculated_response =
    FilterApproximation::calculate_frequency_response(filters, frequencies);

  REQUIRE(calculated_response == reference_response);
}
