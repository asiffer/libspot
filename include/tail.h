/**
 * @file tail.h
 * @brief Declares Tail structure and methods
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU General Public License version 3
 *
 */

#include "estimator.h"
#include "peaks.h"

#ifndef TAIL_H
#define TAIL_H

/**
 * @brief Stucture that embeds GPD parameter (GPD tail actually)
 *
 */
struct Tail {
    double gamma;       /**< GPD gamma parameter */
    double sigma;       /**< GPD sigma parameter */
    struct Peaks peaks; /**< underlying Peaks structure */
};

/**
 * @brief Initialize the tail structure
 *
 * @param tail Tail instance
 * @param size Tail size
 * @return 0 if the initialization is ok
 */
int tail_init(struct Tail *tail, unsigned long size);

/**
 * @brief Free the tail structure
 *
 * @param tail Tail instance
 */
void tail_free(struct Tail *tail);

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
