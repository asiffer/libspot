/**
 * @file tail.h
 * @brief Declares Tail structure and methods
 * @author Alban Siffer (31479857+asiffer@users.noreply.github.com)
 * @version 3.0a0
 * @date mar. 14 avril 2026 14:59:11 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */

#include "estimator.h"

#ifndef TAIL_H
#define TAIL_H

/**
 * @brief Initialize the tail structure
 *
 * @param tail Tail instance
 * @param buffer Buffer to store the tail data
 * @param size Size of the buffer (number of elements)
 * @return 0 always returns 0
 */
int tail_init(struct Tail *tail, double *buffer, unsigned long size);

/**
 * @brief Add a new data into the tail
 *
 * @param tail Tail instance
 * @param x new data
 */
void tail_push(struct Tail *tail, double x);

/**
 * @brief Compute the probability to be higher a given value z
 *
 * @param tail Tail instance
 * @param s the ratio Nt/n (an estimator of P(X>t) = 1-F(t))
 * @param d the local quantile (z_q - t)
 * @return the desired probability
 */
double tail_probability(struct Tail const *tail, double s, double d);

/**
 * @brief Compute the extreme quantile related to the input probability
 *
 * @param tail Tail instance
 * @param s the ratio Nt/n (an estimator of P(X>t) = 1-F(t))
 * @param q the desired [low] probability
 * @return The desired quantile
 */
double tail_quantile(struct Tail const *tail, double s, double q);

/**
 * @brief Defines gamma and sigma for the underlying peaks structure
 *
 * @param tail Tail instance
 * @return The related log-likelihood
 */
double tail_fit(struct Tail *tail);

#endif // TAIL_H
