#include "catch.hpp"
#include "brent.h"

double linear(double x)
{
    INFO("x = " << x);
    return (2.0 * x - 1.0);
}

double square(double x)
{
    return x * x - 4;
}

TEST_CASE("Linear", "[brent]")
{
    const double epsilon = 1e-8;
    const double delta = 1e-6;
    bool found = false;
    function<double(double)> f = linear;
    REQUIRE(f(0.5) == 0.0);
    double root = brent(&found, -1., 2., f, epsilon, delta);
    REQUIRE(found);
    REQUIRE(abs(root - 0.5) < epsilon);
}

TEST_CASE("Square", "[brent]")
{
    const double epsilon = 1e-8;
    const double delta = 1e-6;
    bool found = false;
    function<double(double)> f = square;
    REQUIRE(f(2) == 0.0);
    double root = brent(&found, 0., 10., f, epsilon, delta);
    REQUIRE(found);
    REQUIRE(abs(root - 2) < epsilon);
}

TEST_CASE("Bad initialization", "[brent]")
{
    const double epsilon = 1e-8;
    const double delta = 1e-6;
    bool found = false;
    function<double(double)> f = linear;
    brent(&found, 1., 2., f, epsilon, delta);
    REQUIRE(!found);
}