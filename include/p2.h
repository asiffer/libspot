/**
 * @file p2.h
 * @brief P2 quantile computation
 * @author Alban Siffer (31479857+asiffer@users.noreply.github.com)
 * @version 3.0.0
 * @date mar. 14 avril 2026 14:59:11 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */

#ifndef P2_H
#define P2_H

#include "xmath.h"

/**
 * @brief Compute the p-quantile of the data using the P² algorithm
 *
 * @param p Quantile to compute (0 < p < 1)
 * @param data Array of data points
 * @param size Number of data points
 * @return double The computed p-quantile
 * @retval _NAN if size < 5
 */
double p2_quantile(double p, double const *data, unsigned long size);

#endif // P2_H
