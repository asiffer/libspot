/**
 * @file tail.h
 * @author Alban Siffer
 * @brief
 * @version 0.1
 * @date 2022-08-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "estimator.h"
#include "peaks.h"

#ifndef TAIL_H
#define TAIL_H

/**
 * @brief
 *
 */
struct Tail {
    double gamma;       // GPD parameter
    double sigma;       // GPD parameter
    struct Peaks peaks; // underlying set of peaks
};

/**
 * @brief
 *
 * @param tail
 * @param size
 * @return int
 */
int tail_init(struct Tail *tail, unsigned long size);

/**
 * @brief
 *
 * @param tail
 */
void tail_free(struct Tail *tail);

/**
 * @brief
 *
 * @param tail
 * @param x
 */
void tail_push(struct Tail *tail, double x);

/**
 * @brief Compute the probability to be higher a given value z
 *
 * @param tail the tail structure
 * @param s the ratio Nt/n (an estimator of P(X>t) = 1-F(t))
 * @param d the local quantile (z_q - t)
 * @return an estimate of P(X>z_q)
 */
double tail_probability(struct Tail const *tail, double s, double d);

/**
 * @brief Compute the extreme quantile related to the input probability
 *
 * @param tail the tail structure
 * @param s the ratio Nt/n (an estimator of P(X>t) = 1-F(t))
 * @param q the desired [low] probability
 * @return double z_q such that P(X>z_q) = q
 */
double tail_quantile(struct Tail const *tail, double s, double q);

/**
 * @brief Defines gamma and sigma for the underlying peaks structure
 *
 * @param tail
 * @return double the related log-likelihood
 */
double tail_fit(struct Tail *tail);

#endif // TAIL_H
