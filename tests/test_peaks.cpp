#include "catch.hpp"
#include "peaks.h"
#include "utils.h"

TEST_CASE("Peaks initialization", "[peaks]")
{
    auto peaks = Peaks();
    peaks.set_gpd_parameters(0.0, 1);
}

TEST_CASE("MinMaxSum", "[peaks]")
{

    double min = 0.0;
    double max = 20.0;
    double sum = 0.0;
    const int size = int(max - min) + 1;
    auto peaks = Peaks(size);
    for (double f = min; f <= max; f++)
    {
        peaks.push(f);
        sum += f;
    }
    REQUIRE(peaks.min() == min);
    REQUIRE(peaks.max() == max);
    REQUIRE(peaks.sum() == sum);
}

TEST_CASE("MeanVar", "[peaks]")
{
    double min = 0.0;
    double max = 20.0;
    const int size = int(max - min);
    auto peaks = Peaks(size);
    for (double f = min; f < max; f++)
    {
        peaks.push(f);
    }
    REQUIRE(peaks.mean() == (min + max - 1) / 2);
    REQUIRE(peaks.var() == 33.25);
}

TEST_CASE("log-likelihood", "[peaks]")
{
    const int size = 1000;
    const double pt = 0.95;

    // uniform
    double t = pt;
    auto peaks = uniform_peaks(pt, 0, 1, size);

    REQUIRE(peaks.log_likelihood(0, 1) < peaks.log_likelihood(-1, 1 - t));

    t = gaussian_inv_cdf(pt);
    peaks = gaussian_peaks(pt, 0, 1, size);
    REQUIRE(peaks.log_likelihood(0, peaks.mean()) > peaks.log_likelihood(-0.1, 1));
}

TEST_CASE("Probability", "[peaks]")
{
    const int size = 1000;
    const double proba = 0.01;
    const double pt = 0.95;
    const int n = int(size / (1.0 - pt));

    // uniform
    double t = pt;
    auto peaks = uniform_peaks(pt, 0, 1, size);
    // in the uniform case
    // γ = -1 and σ = 1 - t
    peaks.set_gpd_parameters(-1, 1 - t);
    double p = peaks.probability(1 - proba, t, n, size);
    REQUIRE(abs(p - proba) < 0.01);

    t = gaussian_inv_cdf(pt);
    peaks = gaussian_peaks(pt, 0, 1, size);
    peaks.set_gpd_parameters(0, peaks.mean());
    double z = gaussian_inv_cdf(1 - proba);
    p = peaks.probability(z, t, n, size);
    REQUIRE(abs(p - proba) <= 0.01);
}

TEST_CASE("Quantile", "[peaks]")
{
    const int size = 5000;
    const double proba = 0.01;
    const double pt = 0.95;
    const int n = int(size / (1.0 - pt));

    // uniform
    double t = pt;
    auto peaks = uniform_peaks(pt, 0, 1, size);
    // in the uniform case
    // γ = -1 and σ = 1 - t
    peaks.set_gpd_parameters(-1, 1 - t);
    double q = peaks.quantile(proba, t, n, size);
    INFO("pt = " << pt);
    INFO(" t = " << t);
    INFO(" n = " << n);
    INFO(" q = " << q);
    REQUIRE(abs(q - (1. - proba)) < 0.01);

    // gaussian
    t = gaussian_inv_cdf(pt);
    peaks = gaussian_peaks(pt, 0, 1, size);
    peaks.set_gpd_parameters(0, peaks.mean());
    q = peaks.quantile(proba, t, n, size);

    INFO("pt = " << pt);
    INFO(" t = " << t);
    INFO(" n = " << n);
    INFO(" q = " << q);
    REQUIRE(abs(q - gaussian_inv_cdf(1. - proba)) < 0.02);
}