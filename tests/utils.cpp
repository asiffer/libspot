#include "utils.h"

// random stuff (for testing only)
std::random_device rd;
std::default_random_engine gen(rd());

double alea()
{
    std::uniform_real_distribution<double> u(0, 1);
    return u(gen);
}

vector<double> uniform(double a, double b, int N)
{
    vector<double> v(N);
    std::uniform_real_distribution<double> u(a, b);

    for (int i = 0; i < N; i++)
    {
        v[i] = u(gen);
    }

    return v;
}

vector<double> gaussian(double mu, double sigma, int N)
{
    vector<double> v(N);
    std::normal_distribution<double> g(mu, sigma);

    for (int i = 0; i < N; i++)
    {
        v[i] = g(gen);
    }

    return v;
}

vector<double> lognormal(double m, double s, int N)
{
    vector<double> v(N);
    lognormal_distribution<double> l(m, s);

    for (int i = 0; i < N; i++)
    {
        v[i] = l(gen);
    }
    return v;
}

Peaks uniform_peaks(double p, double a, double b, int N)
{
    auto peaks = Peaks(N);
    double t = a + p * (b - a);

    const int size = min(10, int(1 / (1 - p))) * N;
    vector<double> data = uniform(a, b, size);
    int i = 0;

    UBENDSTATUS status = NOT_FILLED;
    while (status != JUST_FILLED)
    {
        // reset and generate new data
        if (i == N)
        {
            data = uniform(a, b, size);
            i = 0;
        }

        if (data[i] > t)
        {
            status = peaks.push(data[i] - t);
        }

        i++;
    }

    return peaks;
}

double rational_approximation(double t)
{
    // Abramowitz and Stegun formula 26.2.23.
    // The absolute value of the error should be less than 4.5 e-4.
    double c[] = {2.515517, 0.802853, 0.010328};
    double d[] = {1.432788, 0.189269, 0.001308};
    return t - ((c[2] * t + c[1]) * t + c[0]) /
                   (((d[2] * t + d[1]) * t + d[0]) * t + 1.0);
}

double gaussian_inv_cdf(double p)
{
    // See article above for explanation of this section.
    if (p < 0.5)
    {
        // F^-1(p) = - G^-1(p)
        return -rational_approximation(sqrt(-2.0 * log(p)));
    }
    else
    {
        // F^-1(p) = G^-1(1-p)
        return rational_approximation(sqrt(-2.0 * log(1 - p)));
    }
}

Peaks gaussian_peaks(double p, double mu, double sigma, int N)
{
    auto peaks = Peaks(N);
    double t = (gaussian_inv_cdf(p) - mu) / sigma;

    const int size = min(10 * N, int(N / (1 - p)));
    vector<double> data = gaussian(mu, sigma, size);
    int i = 0;

    UBENDSTATUS status = NOT_FILLED;
    while (status != JUST_FILLED)
    {
        // reset and generate new data
        if (i == N)
        {
            data = gaussian(mu, sigma, size);
            i = 0;
        }

        if (data[i] > t)
        {
            status = peaks.push(data[i] - t);
        }

        i++;
    }

    return peaks;
}

int rand_int(int a, int b)
{
    double d = alea() * double(b - a);
    return a + int(d);
}

bool rand_bool()
{
    return alea() < 0.5;
}

double rand_double(double a, double b)
{
    double d = alea() * (b - a);
    return a + d;
}

SpotConfig rand_config()
{
    SpotConfig config;
    config.q = rand_double(0.95, 1.0);
    config.bounded = rand_bool();
    config.max_excess = rand_int(100, 500);
    config.alert = rand_bool();
    config.up = rand_bool();
    config.down = rand_bool();
    config.n_init = rand_int(1000, 5000);
    config.level = rand_double(0.9, 0.95);
    return config;
}