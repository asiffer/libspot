#include "catch_amalgamated.hpp"
#include "ubend.h"

TEST_CASE("Factorials are computed", "[ubend]")
{
    const int size = Catch::rngSeed() % 1000;
    auto ubend = Ubend(size);
    REQUIRE(ubend.getCapacity() == size);
}

TEST_CASE("FXXX", "[ubend]")
{
    const int size = Catch::rngSeed() % 1000;
    auto ubend = Ubend(size);
    REQUIRE(ubend.getCursor() == 0);
}