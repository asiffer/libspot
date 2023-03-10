/**
 * @file peaks.h
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @brief
 * @version 2.0a
 * @date Fri Mar 10 09:44:55 AM UTC 2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "brent.h"
#include "ubend.h"
#include "xmath.h"

#ifndef PEAKS_H
#define PEAKS_H

/**
 * @brief
 *
 */
struct Peaks {
    double e;               /**< sum of the elements */
    double e2;              /**< sum of the square of the elements */
    double min;             /**< min of the elements */
    double max;             /**< max of the elements */
    struct Ubend container; /**< underlying data */
};

/**
 * @brief Initialize the Peaks structure
 *
 * @param peaks Peaks instance
 * @param size Maximum number of peaks to store
 */
int peaks_init(struct Peaks *peaks, unsigned long size);

/**
 * @brief Free the peaks structure
 *
 * @param peaks Peaks instance
 */
void peaks_free(struct Peaks *peaks);

/**
 * @brief Insert a new peak
 *
 * @param peaks Peaks instance
 */
void peaks_push(struct Peaks *peaks, double x);

/**
 * @brief Get the mean of the peaks
 *
 * @param peaks Peaks instance
 * @return the peaks mean
 */
double peaks_mean(struct Peaks const *peaks);

/**
 * @brief Get the variance of the peaks
 *
 * @param peaks Peaks instance
 * @return the peaks variance
 */
double peaks_var(struct Peaks const *peaks);

/**
 * @brief Return the number of peaks
 *
 * @param peaks Peaks instance
 * @return the number of peaks
 */
unsigned long peaks_size(struct Peaks const *peaks);

/**
 * @brief Compute the GPD log-likelihood function
 *
 * @param[in] peaks Peaks instance
 * @return the log likelihood
 */
double log_likelihood(struct Peaks const *peaks, double gamma, double sigma);

#endif // PEAKS_H
