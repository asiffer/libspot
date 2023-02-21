/**
 * \file brent.h
 * \brief Brent's method implementation
 * \author asr
 *
 * \see https://en.wikipedia.org/wiki/Brent%27s_method
 *
 */

#ifndef BRENT_H
#define BRENT_H

extern double const BRENT_DEFAULT_EPSILON;

typedef double (*real_function)(double, void *);

/**
    \brief Root search of a scalar function with the Brent's method
    \param[out] found pointer to retrieve the success of the method
    \param[in] a left bound of the interval
    \param[in] b right bound of the interval
    \param[in] f function of interest
    \param[in] extra function extra parameters
    \param[in] epsilon extra parameter (1e-6)
    \return root
*/
double brent(int *found, double a, double b, real_function f, void *extra,
             double epsilon);

#endif // BRENT_H
