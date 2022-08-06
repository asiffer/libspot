#include "catch.hpp"
#include "estimator.h"
#include "utils.h"

TEST_CASE("Estimator initialization", "[estimator]")
{
    const int size = 1000;
    Peaks peaks = Peaks(size);
    vector<double> data = uniform(0, 0.1, size);
    for (const double x : data)
    {
        peaks.push(x);
    }

    // const auto p = peaks;
    auto g = GrimshawEstimator(peaks);
    auto m = MOMEstimator(peaks);
}

TEST_CASE("Gaussian", "[estimator]")
{
    const int size = 15000;
    double pt = 0.98;
    auto peaks = gaussian_peaks(pt, 0, 1, size);

    INFO(" mean = " << peaks.mean());
    INFO("  var = " << peaks.var());
    INFO("E^2/V = " << peaks.mean() * peaks.mean() / peaks.var());

    auto g = GrimshawEstimator(peaks);
    g.estimate();
    INFO("γ = " << g.gamma);
    INFO("σ = " << g.sigma);
    REQUIRE(abs(g.gamma) < 0.15);
    REQUIRE(abs(g.sigma - peaks.mean()) < 0.15);

    auto m = MOMEstimator(peaks);
    m.estimate();
    INFO("γ = " << m.gamma);
    INFO("σ = " << m.sigma);
    REQUIRE(abs(m.gamma) < 0.15);
    REQUIRE(abs(m.sigma - peaks.mean()) < 0.15);
}

TEST_CASE("Uniform", "[estimator]")
{
    const int size = 15000;
    double pt = 0.9;
    double t = pt;
    auto peaks = uniform_peaks(pt, 0, 1, size);

    INFO(" mean = " << peaks.mean());
    INFO("  var = " << peaks.var());
    INFO("E^2/V = " << peaks.mean() * peaks.mean() / peaks.var());

    auto g = GrimshawEstimator(peaks);
    g.estimate();
    INFO("γ = " << g.gamma);
    INFO("σ = " << g.sigma);
    // REQUIRE(abs(g.gamma - (-1.)) < 0.05);
    REQUIRE(g.gamma <= 0.);
    REQUIRE(abs(g.sigma - (1 - t)) < 0.15);

    auto m = MOMEstimator(peaks);
    m.estimate();
    INFO("γ = " << m.gamma);
    INFO("σ = " << m.sigma);
    REQUIRE(abs(m.gamma - (-1.)) < 0.05);
    REQUIRE(abs(m.sigma - (1 - t)) < 0.15);
}

TEST_CASE("GPD fit", "[estimator]")
{
    const int size = 15000;
    double pt = 0.9;
    // double t = pt;
    auto peaks = uniform_peaks(pt, 0, 1, size);

    INFO(" mean = " << peaks.mean());
    INFO("  var = " << peaks.var());
    INFO("E^2/V = " << peaks.mean() * peaks.mean() / peaks.var());

    double gamma = -1.;
    double sigma = -1.;

    auto estimators = gpd_fit(peaks, &gamma, &sigma);
}