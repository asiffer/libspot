
#include <vector>
#include <random>
#include "peaks.h"
#include "spot.h"

using namespace std;

#ifndef UTILS_H
#define UTILS_H

vector<double> uniform(double a, double b, int N);

vector<double> gaussian(double mu, double sigma, int N);

vector<double> lognormal(double m, double s, int N);

double gaussian_inv_cdf(double p);

Peaks uniform_peaks(double p, double a, double b, int N);

Peaks gaussian_peaks(double p, double mu, double sigma, int N);

Peaks lognormal_peaks(double p, double m, double s, int N);

int rand_int(int a, int b);

bool rand_bool();

double rand_double(double a, double b);

SpotConfig rand_config();

#endif // UTILS_H