#include "spot.h"
#include "utils.h"
#include "catch.hpp"

TEST_CASE("Default initialization", "[spot]")
{
    auto s0 = Spot();
    const SpotConfig c0 = s0.config();
    REQUIRE(c0.q == 0.001);
    REQUIRE(c0.bounded == true);
    REQUIRE(c0.max_excess == 200);
    REQUIRE(c0.alert == true);
    REQUIRE(c0.up == true);
    REQUIRE(c0.down == true);
    REQUIRE(c0.n_init == 1000);
    REQUIRE(c0.level == 0.99);
}

TEST_CASE("Initialization 1", "[spot]")
{
    const int size = 2500;
    const double q = 0.0005;
    vector<double> sample = gaussian(0., 1., size);
    auto s0 = Spot(q, sample);
    const SpotConfig c0 = s0.config();
    REQUIRE(c0.q == q);
    REQUIRE(c0.bounded == true);
    REQUIRE(c0.max_excess == 200);
    REQUIRE(c0.alert == true);
    REQUIRE(c0.up == true);
    REQUIRE(c0.down == true);
    REQUIRE(c0.n_init == size);
    REQUIRE(c0.level == 0.99);
}

TEST_CASE("Initialization 2", "[spot]")
{
    const auto config = rand_config();
    vector<double> sample = gaussian(0., 1., config.n_init);
    auto s0 = Spot(
        config.q,
        sample,
        config.level,
        config.up,
        config.down,
        config.alert,
        config.bounded,
        config.max_excess);

    const SpotConfig c0 = s0.config();
    REQUIRE(c0.q == config.q);
    REQUIRE(c0.bounded == config.bounded);
    REQUIRE(c0.max_excess == config.max_excess);
    REQUIRE(c0.alert == config.alert);
    REQUIRE(c0.up == config.up);
    REQUIRE(c0.down == config.down);
    REQUIRE(c0.n_init == config.n_init);
    REQUIRE(c0.level == config.level);
}

TEST_CASE("Initialization 3", "[spot]")
{
    const auto config = rand_config();
    auto s0 = Spot(config);
    const SpotConfig c0 = s0.config();
    REQUIRE(c0.q == config.q);
    REQUIRE(c0.bounded == config.bounded);
    REQUIRE(c0.max_excess == config.max_excess);
    REQUIRE(c0.alert == config.alert);
    REQUIRE(c0.up == config.up);
    REQUIRE(c0.down == config.down);
    REQUIRE(c0.n_init == config.n_init);
    REQUIRE(c0.level == config.level);
}

TEST_CASE("Initialization 4", "[spot]")
{
    auto config = rand_config();
    config.down = true;
    config.up = true;
    const int n_init = -500;
    auto s0 = Spot(
        config.q,
        n_init,
        config.level,
        config.up,
        config.down,
        config.alert,
        config.bounded,
        config.max_excess);
    auto c0 = s0.config();
    REQUIRE(c0.q == config.q);
    REQUIRE(c0.bounded == config.bounded);
    REQUIRE(c0.max_excess == config.max_excess);
    REQUIRE(c0.alert == config.alert);
    REQUIRE(c0.up == config.up);
    REQUIRE(c0.down == config.down);
    REQUIRE(c0.n_init == 1000);
    REQUIRE(c0.level == config.level);
}

TEST_CASE("Equality", "[spot]")
{
    const auto config = rand_config();
    auto s0 = Spot(config);
    auto s1 = Spot(config);
    REQUIRE(s0 == s1);
}

TEST_CASE("Init steps", "[spot]")
{
    const double q = 0.001;
    const int n_init = 5000;
    const int check_period = n_init / 10;
    const double level = 0.98;
    const bool up = true, down = true, alert = true, bounded = true;
    const int max_excess = 100;

    vector<double> init_data = gaussian(0, 1, 5000);

    SPOTEVENT event;

    auto s0 = Spot(q, n_init, level, up, down, alert, bounded, max_excess);

    int k = 0;
    for (auto &x : init_data)
    {
        event = s0.step(x);
        // periodic check
        if (k % check_period == 0)
        {
            REQUIRE(event == SPOTEVENT::INIT_BATCH);
        }
        k++;
    }
    // the last event must be the calibration one
    REQUIRE(event == SPOTEVENT::CALIBRATION);

    // check the status
    const auto status = s0.status();
    REQUIRE(status.n == n_init);
    const double z = (1. - level) * n_init;
    INFO("z: " << z);
    INFO("Nt up: " << status.Nt_up);
    INFO("Nt down: " << status.Nt_down);
    REQUIRE(abs(status.Nt_up - z) / z <= 0.011);
    REQUIRE(abs(status.Nt_down - z) / z <= 0.011);
}

TEST_CASE("Cruising steps", "[spot]")
{
    const double q = 0.001;
    const int n_init = 20000, n_next = 80000;

    const double level = 0.98;
    const bool up = true, down = true, alert = true, bounded = false;
    const int max_excess = -500;

    vector<double> init_data = gaussian(0, 1, n_init);
    vector<double> next_data = gaussian(0, 1, n_next);

    const int check_period = n_next / 10;

    SPOTEVENT event;

    auto s0 = Spot(q, init_data, level, up, down, alert, bounded, max_excess);
    auto status = s0.status();

    REQUIRE(status.n == n_init);

    int k = 0;
    for (auto &x : next_data)
    {
        event = s0.step(x);
        // periodic check
        if (k % check_period == 0)
        {
            REQUIRE(event != SPOTEVENT::INIT_BATCH);
            REQUIRE(event != SPOTEVENT::CALIBRATION);
        }
        k++;
    }

    // check the status
    status = s0.status();
    const int n = init_data.size() + next_data.size();
    // we expect a number of alarms lower than q*n for
    // both tails
    REQUIRE(double(n - status.n) / (double)n < 10 * q);
    const double z = (1. - level) * status.n;
    INFO("z: " << z);
    INFO("Nt up: " << status.Nt_up);
    INFO("Nt down: " << status.Nt_down);
    REQUIRE(abs(status.Nt_up - z) / z <= 0.15);
    REQUIRE(abs(status.Nt_down - z) / z <= 0.15);
}

TEST_CASE("Queries", "[spot]")
{
    // here we do not trigger alerts
    const double q = 0.001;
    const int n_init = 10000, n_next = 90000;

    const double level = 0.98;
    const bool up = true, down = true, alert = false, bounded = false;
    const int max_excess = -500;

    vector<double> init_data = gaussian(0, 1, n_init);
    vector<double> next_data = gaussian(0, 1, n_next);

    auto s0 = Spot(2 * q, init_data, level, up, down, alert, bounded, max_excess);
    s0.set_q(q);
    REQUIRE(s0.config().q == q);

    for (auto &x : next_data)
    {
        s0.step(x);
    }

    double t_down = gaussian_inv_cdf(1. - level);
    double t_up = gaussian_inv_cdf(level);

    INFO("t_down: " << s0.getLower_t());
    INFO("t_up: " << s0.getUpper_t());
    REQUIRE(abs((s0.getLower_t() - t_down) / t_down) < 0.05);
    REQUIRE(abs((s0.getUpper_t() - t_up)) / t_up < 0.05);

    double z_up = gaussian_inv_cdf(1. - q);
    double z_down = gaussian_inv_cdf(q);

    INFO("z_down: " << s0.getLowerThreshold());
    INFO("z_up: " << s0.getUpperThreshold());
    REQUIRE(abs((s0.getLowerThreshold() - z_down) / z_down) < 0.05);
    REQUIRE(abs((s0.getUpperThreshold() - z_up)) / z_up < 0.05);

    auto bounds = s0.getThresholds();
    REQUIRE(bounds.down == s0.getLowerThreshold());
    REQUIRE(bounds.up == s0.getUpperThreshold());

    REQUIRE(abs(s0.up_probability(s0.getUpperThreshold()) - q) < 0.01);
    REQUIRE(abs(s0.down_probability(s0.getLowerThreshold()) - q) < 0.01);
}

TEST_CASE("Print", "[spot]")
{
    // here we do not trigger alerts
    const double q = 0.001;
    const int n_init = 10000, n_next = 90000;

    const double level = 0.98;
    const bool up = true, down = true, alert = false, bounded = false;
    const int max_excess = -500;

    vector<double> init_data = gaussian(0, 1, n_init);
    vector<double> next_data = gaussian(0, 1, n_next);

    auto s0 = Spot(q, init_data, level, up, down, alert, bounded, max_excess);

    for (auto &x : next_data)
    {
        s0.step(x);
    }

    s0.log(2);
    s0.stringStatus();
    s0.config().str();
}