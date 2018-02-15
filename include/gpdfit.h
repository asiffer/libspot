/**
	\file gpdfit.h
	\brief GPDfit class implementation
	\details This class implement the Generalized Pareto Distribution fit given some data
	\author asr
	\version 1.0
*/



#include "ubend.h"
#include "brent.h"
#include <vector>
#include <iostream>
#include <functional>
#include <cmath>
#include <algorithm>
#include <numeric>


using namespace std;


#ifndef GPDINFO_H
#define GPDINFO_H

/*!
 *	\struct GPDinfo
 *  	\brief The GPDinfo structure.
 *	\details This structure gathers the information about a GPD fit (parameters + log-likelihood)
 */
struct GPDinfo {
    double gamma;	/*!< first parameter of the GPD */
    double sigma;	/*!< second parameter of the GPD */
    double llhood;	/*!< the corresponding log-likelihood */
};

#endif // GPDINFO_H



#ifndef GPDFIT_H
#define GPDFIT_H

/*!
 *	\class GPDfit
 *	\brief This class implement the Generalized Pareto Distribution fit given some data.
 *	\details This class is based on the Ubend structure.
 *	It performs maximum likelihood estimation of the GPD parameters with the Grimshaw's trick
 */
class GPDfit : public Ubend {
    private:
        // Methods (root search)
	/*!
		\brief Auxiliary function to compute the likelihood
		\param[in] x the antecedent
		\return v(x) 
	*/
        double grimshaw_v(double x);
        
	/*!
		\brief simplified log-likelihood function
		\param[in] x_star the antecedent
		\return GPDinfo object gathering gamma, sigma and the likelihood 
	*/
        GPDinfo log_likelihood(double x_star);
        

	/*!
		\brief Return the roots of the auxiliary function w
	*/
        vector<double> roots();

    public:

        /**
			\brief Constructor
			\param[in] size_max maximum number of values to store
			\return GPDfit object
		*/
        GPDfit(int capacity = -1);
        
        /*!
		\brief Get the minimum value of stored data
	*/
        double min();
        
        /*!
		\brief Get the maximum value of stored data
	*/
        double max();
        
        /*!
		\brief Get the mean of stored data
	*/
        double mean();
        
	/*!
		\brief Auxiliary function to compute the likelihood
		\param[in] x the antecedent
		\return w(x) = u(x)v(x) - 1 
	*/
        double grimshaw_w(double x);
        
	/*!
		\brief Perform a GPD fit of the stored excesses
		\return GPDinfo object gathering gamma, sigma and the likelihood 
	*/
        GPDinfo fit();

};

#endif // GPDFIT_H



