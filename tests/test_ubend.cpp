#include "catch.hpp"
#include "ubend.h"

int get_short_seed()
{
    int z = Catch::rngSeed();
    if (z == 0)
    {
        z = 10; // default value
    }
    if (z < 0)
    {
        z = -z;
    }
    return z % 1000;
}

TEST_CASE("Ubend initialization", "[ubend]")
{
    const int size = get_short_seed();
    auto ubend = Ubend(size);
    REQUIRE(ubend.getCapacity() == size);
    REQUIRE(ubend.getCursor() == 0);
    REQUIRE(ubend.isFilled() == false);
    REQUIRE(std::isnan(ubend.getLastErasedData()));
}

TEST_CASE("Dynamic use infinite", "[ubend]")
{
    const int size = -10;
    auto ubend = Ubend(size);
    REQUIRE(ubend.getCapacity() == -1);
    for (int i = 0; i < 20; i++)
    {
        REQUIRE(ubend.push(double(i)) == UBENDSTATUS::INFINITE);
    }
}

TEST_CASE("Dynamic use finite", "[ubend]")
{
    const int size = 10;
    auto ubend = Ubend(size);
    for (int i = 0; i < size - 1; i++)
    {
        REQUIRE(ubend.push(double(i)) == UBENDSTATUS::NOT_FILLED);
    }
    REQUIRE(ubend.push(double(size - 1)) == UBENDSTATUS::JUST_FILLED);
    REQUIRE(ubend.push(double(size)) == UBENDSTATUS::CRUISING);
    REQUIRE(ubend.getLastErasedData() == 0);
}

TEST_CASE("Merge", "[ubend]")
{
    const int size = 10;
    auto ubend10 = Ubend(size);
    auto ubend20 = Ubend(2 * size);
    for (double f = 0.0; f < 10.0; f += 1.0)
    {
        ubend10.push(f);
        ubend20.push(2 * f);
    }
    auto merged = ubend10.merge(ubend20);
    REQUIRE(merged.getCapacity() == size);
    REQUIRE(merged.getCursor() == 0);
}