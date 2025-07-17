/**
 * @file brent.h
 * @brief Brent's method declaration
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b4
 * @date jeu. 17 juil. 2025 08:08:51 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */

#ifndef BRENT_H
#define BRENT_H

#include "structs.h"

extern double const BRENT_DEFAULT_EPSILON;

extern unsigned long const BRENT_ITMAX;

/**
    \brief Root search of a scalar function with the Brent's method. It uses
   the code from "Numerical Recipes: The Art of Scientific Computing".
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
