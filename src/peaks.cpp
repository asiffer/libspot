#include "peaks.h"

using namespace std;

/**
 * @brief Construct a new Peaks object
 *
 * @param capacity maximum number of values to store
 */
Peaks::Peaks(int capacity)
	: Ubend(capacity)
{
	this->gamma = 0.0;
	this->sigma = 0.0;
}

/**
 *	@brief Get the minimum value of stored data
 */
double Peaks::min() const
{
	return *std::min_element(this->begin(), this->end());
}

/**
 *	@brief Get the maximum value of stored data
 */
double Peaks::max() const
{
	return *std::max_element(this->begin(), this->end());
}

/**
 *	@brief Return the mean of the stored excesses
 */
double Peaks::sum() const
{
	return std::accumulate(this->begin(), this->end(), 0.0);
}
/**
 *	@brief Return the mean of the stored excesses
 */
double Peaks::mean() const
{
	return this->sum() / double(this->size());
}

/**
 *	@brief Return the variance of the stored excesses
 */
double Peaks::var() const
{
	double ss = 0.0;
	for (const auto Yi : *this)
	{
		ss += Yi * Yi;
	}
	// for (auto i = this->begin(); i != this->end(); i++)
	// {
	// 	ss += pow(*i, 2);
	// }
	return ((ss / double(this->size())) - pow(this->mean(), 2));
}

/* ====================== Stuff specific to GPD  ====================== */

/**
 * @brief set gamma et sigma parameters
 */
void Peaks::set_gpd_parameters(double gamma, double sigma)
{
	this->gamma = gamma;
	this->sigma = sigma;
}

/**
 *	@brief compute the GPD log-likelihood function
 *	@param[in] g pointer to gamma parameter
 *	@param[in] s pointer to sigma parameter
 *	@return log-likelihood
 */
double Peaks::log_likelihood(double gamma, double sigma) const
{
	const double Nt_local = this->size();
	if (gamma == 0.)
	{
		return -Nt_local * std::log(sigma) - this->sum() / sigma;
	}

	double r = -Nt_local * std::log(sigma);
	const double c = 1. + 1. / gamma;
	const double x = gamma / sigma;
	for (size_t i = 0; i < this->size(); i++)
	{
		r += -c * std::log(1 + x * this->at(i));
	}
	return r;
}

/**
 *	@brief Given some extra parameters, compute the probability 1-F(z)
 *	@param z Desired quantile
 *	@param t Intermediate threshold
 *	@param n Total number of observations
 *	@param Nt Total number of observations above t
 */
double Peaks::probability(double z, double t, int n, int Nt) const
{
	double r = (z - t) * (this->gamma / this->sigma);
	double s = 1. * Nt / n;
	if (this->gamma == 0.0)
	{
		return s * std::exp(-(z - t) / this->sigma);
	}
	else
	{
		return s * pow(1 + r, -1.0 / this->gamma);
	}
}

/**
 *	@brief Given some extra parameters, compute the quantile z such that F(z) = 1-q
 *	@param q Desired probability
 *	@param t Intermediate threshold
 *	@param n Total number of observations
 *	@param Nt Total number of observations above t
 */
double Peaks::quantile(double q, double t, int n, int Nt) const
{
	double r = (q * n) / Nt;
	if (this->gamma == 0.0)
	{
		return (t - this->sigma * std::log(r));
	}
	else
	{
		return t + (this->sigma / this->gamma) * (pow(r, -this->gamma) - 1);
	}
}
