// test_gamma.cpp

#include "spot.h"
#include <random>
#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>

using namespace std;

vector<double> gaussian_white_noise(double mu, double sigma, int N)
{
    vector<double> v(N);
    random_device rd;
    default_random_engine gen(rd());
    normal_distribution<double> gaussian(mu, sigma);

    for (int i = 0; i < N; i++)
    {
        v[i] = gaussian(gen);
    }
    return v;
}

vector<double> uniform_white_noise(double a, double b, int N)
{
    vector<double> v(N);
    random_device rd;
    default_random_engine gen(rd());
    uniform_real_distribution<double> uniform(a, b);

    for (int i = 0; i < N; i++)
    {
        v[i] = uniform(gen);
    }
    return v;
}

vector<double> lognormal_white_noise(double m, double s, int N)
{
    vector<double> v(N);
    random_device rd;
    default_random_engine gen(rd());
    lognormal_distribution<double> lognormal(m, s);

    for (int i = 0; i < N; i++)
    {
        v[i] = lognormal(gen);
    }
    return v;
}

int main(int argc, const char *argv[])
{
    int N = 200000;
    double gamma, sigma;

    auto white_noises = {
        uniform_white_noise(0, 1, N),
        lognormal_white_noise(0, 1, N),
        gaussian_white_noise(0, 1, N),
    };

    for (const auto &data : white_noises)
    {
        Peaks G(N);
        for (const auto &x : data)
        {
            G.push(x);
        }

        gpd_fit(G, &gamma, &sigma);
    }
}
