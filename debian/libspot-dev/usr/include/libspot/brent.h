/**
 * \file brent.h
 * \brief Brent's method implementation 
 * \author asr
 *
 * \see https://en.wikipedia.org/wiki/Brent%27s_method
 *
 */


#include <functional>
#include <cmath>

using namespace std;


#ifndef BRENT_H
#define BRENT_H

/*!
	\brief Root search of a scalar function with the Brent's method
	\param[out] found pointer to retrieve the success of the method
	\param[in] a left bound of the interval
	\param[in] b right bound of the interval
	\param[in] f function of interest
	\param[in] epsilon extra parameter
	\param[in] delta extra parameter
	\return root
*/
double brent(bool * found,
             double a,
             double b,
             function<double(double)> f,
             double epsilon = 1e-6,
             double delta = 1e-8);

             
#endif // BRENT_H
