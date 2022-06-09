#include "gpdfit.h"

using namespace std;

/**
	@brief Constructor
	@param size_max maximum number of values to store
	@return GPDfit object
*/
GPDfit::GPDfit(int capacity)
	: Ubend(capacity)
{
	this->gamma = 0.0;
	this->sigma = 0.0;
	this->llhood = 0.0;
}

/**
	@brief Return the smallest stored excess
*/
double GPDfit::min()
{
	return *std::min_element(this->begin(), this->end());
}

/**
	@brief Return the largest stored excess
*/
double GPDfit::max()
{
	return *std::max_element(this->begin(), this->end());
}

/**
	@brief Return the mean of the stored excesses
*/
double GPDfit::mean()
{
	double sum = std::accumulate(this->begin(), this->end(), 0.0);
	return sum / this->size();
}

/**
	@brief (NEW) Return the variance of the stored excesses
*/
double GPDfit::var()
{
	vector<double>::iterator i;
	int ss = 0;
	for (i = this->begin(); i != this->end(); i++)
	{
		ss += pow(*i, 2);
	}
	return (ss / this->size()) - pow(this->mean(), 2);
}

// fit

/**
	@brief Auxiliary function to compute the likelihood
	@param x the antecedent
	@return v(x)
*/
double GPDfit::grimshaw_v(double x)
{
	double v = 0.0;
#ifdef _OPENMP
#pragma omp parallel for reduction(+ \
								   : v)
#endif
	for (size_t i = 0; i < this->size(); i++)
	{
		v += log(1 + x * this->at(i));
	}
	/*for (auto it = this->begin(); it != this->end(); ++it) {
		v += log(1 + x * (*it));
	}*/
	return (1 + v / this->size());
}

/**
	@brief Auxiliary function to compute the likelihood
	@param x the antecedent
	@return w(x) = u(x)v(x) - 1
*/
double GPDfit::grimshaw_w(double x)
{
	double Nt_local = this->size();
	double u = 0.0;
	double v = 0.0;
	double s;

	/*
	for (auto it = this->begin(); it != this->end(); ++it)
	{
		s = 1 + x * (*it);
		u += 1 / s;
		v += log(s);
	}*/
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (size_t i = 0; i < this->size(); i++)
	{
		s = 1 + x * this->at(i);
		u += 1 / s;
		v += log(s);
	}
	return ((u / Nt_local) * (1 + v / Nt_local) - 1);
}

/**
	@brief simplified log-likelihood function
	@param[in] x_star the antecedent
	@param[out] g pointer to gamma parameter
	@param[out] s pointer to sigma parameter
	@return log-likelihood
*/
double GPDfit::log_likelihood(double x_star, double *g, double *s)
{
	double Nt_local = this->size();
	*g = this->grimshaw_v(x_star) - 1;

	if (*g == 0.0)
	{
		*s = this->mean();
		return (Nt_local * (1 + std::log(*s)));
	}
	else
	{
		*s = *g / x_star;
		return (-Nt_local * std::log(*s) - Nt_local * (1 + *g));
	}
}

/**
	@brief Return the roots of the auxiliary function w
*/
vector<double> GPDfit::roots()
{
	// vector of roots
	vector<double> vec_roots;

	double min = this->min();
	double max = this->max();
	double mean = this->mean();
	double epsilon = std::min(1e-9, 0.5 / max);
	double r;
	double a, b;
	bool found = false;

	// 0 is always a root
	vec_roots.push_back(0.0);

	// the grimshaw function is bound to perform the brent root search
	using std::placeholders::_1;
	function<double(double)> f = std::bind(&GPDfit::grimshaw_w, this, _1);

	// left root
	a = -1 / max + epsilon;
	b = -epsilon;
	r = brent(&found, a, b, f);
	if (found)
	{
		vec_roots.push_back(r);
	}

	// right root
	a = epsilon;
	b = 2 * (mean - min) / (min * min);
	r = brent(&found, a, b, f);
	if (found)
	{
		vec_roots.push_back(r);
	}

	return (vec_roots);
}

/**
	@brief Perform a GPD fit of the stored excesses
*/
void GPDfit::fit()
{
	// variable to retrieve gpdfit results
	double g;
	double s;
	double logL;

	// retrieve the candidates
	vector<double> candidates = this->roots();

	// L = -oo
	double logL0 = -numeric_limits<double>::infinity();

	for (const auto x_star : candidates)
	{
		logL = this->log_likelihood(x_star, &g, &s);
		if (logL > logL0)
		{
			logL0 = logL;
			this->gamma = g;
			this->sigma = s;
			this->llhood = logL;
		}
	}
}

double GPDfit::quantile(double q, double t, int n, int Nt)
{
	double r = (q * n) / Nt;
	if (this->gamma == 0.0)
	{
		return (t - this->sigma * std::log(r));
	}
	else
	{
		return (t + (this->sigma / this->gamma) * (pow(r, -this->gamma) - 1));
	}
}

double GPDfit::probability(double z, double t, int n, int Nt)
{
	double r = (z - t) * (this->gamma / this->sigma);
	double s = 1. * Nt / n;
	if (this->gamma == 0.0)
	{
		return (s * std::exp(-(z - t) / this->sigma));
	}
	else
	{
		return (s * pow(1 + r, -1.0 / this->gamma));
	}
}
