#include "spot.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

/* COLORS FOR FANCY PRINT */
#define END "\x1B[0m"
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"

#define OK "[" GRN "OK" END "]"
#define WARNING "[" YEL "WARNING" END "]"
#define FAIL "[" RED "FAIL" END "]"
/* END OF COLORS FOR FANCY PRINT */

using namespace std;
using T = std::chrono::steady_clock::time_point;
using usec = std::chrono::microseconds;
using sec = std::chrono::seconds;
const auto& tick = std::chrono::steady_clock::now;

int duration(T start, T end)
{
    return chrono::duration_cast<usec>(end - start).count();
}

int operator-(const T& end, const T& start)
{
    return (duration(start, end));
}

double alea()
{
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<double> u(0, 1);
    return u(gen);
}

vector<double> uniform(double a, double b, int N)
{
    vector<double> v(N);
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<double> u(a, b);

    for (int i = 0; i < N; i++) {
        v[i] = u(gen);
    }

    return (v);
}

vector<double> gaussian(double mu, double sigma, int N)
{
    vector<double> v(N);
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::normal_distribution<double> d(mu, sigma);

    for (int i = 0; i < N; i++) {
        v[i] = d(gen);
    }

    return (v);
}

int main(int argc, const char* argv[])
{
    // the tests depend on these parameters
    double mu = 0.0;
    double sigma = 2.0;
    int N = 12000;
    int N_rep = 300;
    cout << "Feeding Spot with " << N << " values ";
#ifdef _OPENMP
    cout << "with OPENMP (" << _OPENMP << ")" << endl;
#else
    cout << "without OPENMP" << endl;
#endif

    double total_time_msec = 0.0;
    vector<double> data = gaussian(mu, sigma, N);
    // ------------------------------------

    for (int i = 0; i < N_rep; i++) {
        data = gaussian(mu, sigma, N);
        //cout << endl << "\t1. Quantile computation" << endl;
        Spot S(1e-3, 2000, 0.99, true, true, false, true, 200);

        T start = tick();
        for (auto x : data) {
            S.step(x);
        }
        T end = tick();
        int us = duration(start, end);
        total_time_msec += us / 1e3;
    }

    cout << "\tAverage computation time (x" << N_rep << " rep.): " << total_time_msec/N_rep << "ms" << endl;
    return 0;
}