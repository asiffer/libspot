/**
	@file gpdfit.h
	@brief Peaks class implementation
	@details This class implement the Generalized Pareto Distribution fit given some data
	@author asr
*/

#include "brent.h"
#include "ubend.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>
#include <limits>

using namespace std;

#ifndef PEAKS_H
#define PEAKS_H

/**
 *	@class Peaks
 *	@brief This class is a specific Ubend container to perform GPD fit.
 */
class Peaks : public Ubend
{
private:
	double gamma;
	double sigma;

public:
	/**
	 * @brief Construct a new Peaks object
	 *
	 * @param capacity maximum number of values to store
	 */
	Peaks(int capacity = -1);

	/**
	 *	@brief Get the minimum value of stored data
	 */
	double min() const;

	/**
	 *	@brief Get the maximum value of stored data
	 */
	double max() const;

	/**
	 *	@brief Get the sum of stored data
	 */
	double sum() const;

	/**
	 * @brief Get the mean of stored data
	 */
	double mean() const;

	/**
	 *	@brief Return the variance of the stored excesses
	 */
	double var() const;

	/* ====================== Stuff specific to GPD  ====================== */

	/**
	 * @brief set gamma et sigma parameters
	 */
	void set_gpd_parameters(double gamma, double sigma);

	/**
	 *	@brief compute the GPD log-likelihood function
	 *	@param[in] g pointer to gamma parameter
	 *	@param[in] s pointer to sigma parameter
	 *	@return log-likelihood
	 */
	double log_likelihood(double gamma, double sigma) const;

	/**
	 *	@brief Given some extra parameters, compute the probability 1-F(z)
	 *	@param z Desired quantile
	 *	@param t Intermediate threshold
	 *	@param n Total number of observations
	 *	@param Nt Total number of observations above t
	 */
	double probability(double z, double t, int n, int Nt) const;

	/**
	 *	@brief Given some extra parameters, compute the quantile z such that F(z) = 1-q
	 *	@param q Desired probability
	 *	@param t Intermediate threshold
	 *	@param n Total number of observations
	 *	@param Nt Total number of observations above t
	 */
	double quantile(double q, double t, int n, int Nt) const;
};

#endif // PEAKS_H
