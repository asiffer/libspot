#include "catch.hpp"
#include "bounds.h"

TEST_CASE("Bounds initialization", "[bounds]")
{
    const double down = 0.0;
    const double up = 1.0;
    auto bounds = Bounds(down, up);
    REQUIRE(bounds.down == down);
    REQUIRE(bounds.up == up);
}

TEST_CASE("Interval", "[bounds]")
{
    const double down = 0.0;
    const double up = 10.0;
    auto bounds = Bounds(down, up);
    for (double f = up; f < 2 * up; f += 1.0)
    {
        REQUIRE(bounds.distanceTo(f) == f - up);
    }
    for (double f = -2 * up; f < down; f += 1.0)
    {
        REQUIRE(bounds.distanceTo(f) == down - f);
        REQUIRE(!bounds.contains(f));
    }
    for (double f = down; f < up; f += 1.0)
    {
        REQUIRE(bounds.distanceTo(f) == 0.0);
        REQUIRE(bounds.contains(f));
    }
}

TEST_CASE("Comparisons", "[bounds]")
{
    auto bounds1 = Bounds(0.0, 10);
    auto bounds2 = Bounds(11.0, 15.0);
    REQUIRE(bounds1 < bounds2);

    bounds1 = Bounds(0.0, 10);
    bounds2 = Bounds(5.0, 15.0);
    REQUIRE(!(bounds1 < bounds2));

    bounds1 = Bounds(5.0, 6.0);
    bounds2 = Bounds(3.0, 4.0);
    REQUIRE(bounds1 > bounds2);

    bounds1 = Bounds(2.0, 6.0);
    bounds2 = Bounds(3.0, 4.0);
    REQUIRE(!(bounds1 > bounds2));
}