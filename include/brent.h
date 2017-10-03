#ifndef BRENT_H
#define BRENT_H

#include <functional>
#include <cmath>

using namespace std;

struct BrentResults
{
	double xmin;
	double fmin;
};

double brent(bool * found,
             double a,
             double b,
             function<double(double)> f,
             double epsilon = 1e-6,
             double delta = 1e-8);
             
BrentResults brent2(bool * found,
             double a,
             double b,
             function<double(double)> f,
             double epsilon = 1e-6,
             double delta = 1e-8);
             
#endif // BRENT_H
