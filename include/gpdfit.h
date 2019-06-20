/**
	\file gpdfit.h
	\brief GPDfit class implementation
	\details This class implement the Generalized Pareto Distribution fit given some data
	\author asr
*/

#include "brent.h"
#include "ubend.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

#ifndef GPDFIT_H
#define GPDFIT_H

/**
 *	\class GPDfit
 *	\brief This class implement the Generalized Pareto Distribution fit given some data.
 *	\details This class is based on the Ubend structure.
 *	It performs maximum likelihood estimation of the GPD parameters with the Grimshaw's trick
 */
class GPDfit : public Ubend {
private:
    double gamma;
    double sigma;
    double llhood;
    // Methods (root search)
    /**
			\brief Auxiliary function to compute the likelihood
			\param[in] x the antecedent
			\return v(x) 
		*/
    double grimshaw_v(double x);

    /**
			\brief simplified log-likelihood function
			\param[in] x_star the antecedent
			\param[out] gamma pointer to the computed gamma parameter
			\param[out] sigma pointer to the computed sigma parameter
			\return the log-likelihood 
		*/
    double log_likelihood(double x_star, double* gamma, double* sigma);
    //GPDinfo log_likelihood(double x_star);

    /**
			\brief Return the roots of the auxiliary function w
		*/
    vector<double> roots();

public:
    /**
			\brief Constructor
			\param[in] capacity maximum number of values to store
			\return GPDfit object
		*/
    GPDfit(int capacity = -1);

    /**
			\brief Get the minimum value of stored data
		*/
    double min();

    /**
		\brief Get the maximum value of stored data
	**/
    double max();

    /**
		\brief Get the mean of stored data
	**/
    double mean();

    /**
		\brief (NEW) Return the variance of the stored excesses
	**/
    double var();

    /**
		\brief Auxiliary function to compute the likelihood
		\param[in] x the antecedent
		\return w(x) = u(x)v(x) - 1 
	**/
    double grimshaw_w(double x);

    /**
			\brief Perform a GPD fit of the stored excesses
			\return GPDinfo object gathering gamma, sigma and the likelihood 
		*/
    void fit();

    /**
			\brief Given some extra parameters, compute the probability 1-F(z)
			\param z Desired quantile
			\param t Intermediate threshold
			\param n Total number of observations
			\param Nt Total number of observations above t
		*/
    double probability(double z, double t, int n, int Nt);

    /**
			\brief Given some extra parameters, compute the quantile z such that F(z) = 1-q
			\param q Desired probability
			\param t Intermediate threshold
			\param n Total number of observations
			\param Nt Total number of observations above t
		*/
    double quantile(double q, double t, int n, int Nt);
};

#endif // GPDFIT_H
