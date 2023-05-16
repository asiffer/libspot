/**
 * @file estimator.h
 * @brief Declares GPD parameters estimators
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */

#include "peaks.h"

#ifndef ESTIMATOR_H
#define ESTIMATOR_H

typedef double (*estimator)(struct Peaks const *, double *, double *);

/**
 * @brief
 *
 * @param peaks Peaks instance
 * @param[out] gamma computed GPD gamma parameter
 * @param[out] sigma computed GPD sigma parameter
 * @return the log-likelihood of the estimation
 */
double mom_estimator(struct Peaks const *peaks, double *gamma, double *sigma);

/**
 * @brief
 *
 * @param peaks Peaks instance
 * @param[out] gamma computed GPD gamma parameter
 * @param[out] sigma computed GPD sigma parameter
 * @return the log-likelihood of the estimation
 */
double grimshaw_estimator(struct Peaks const *peaks, double *gamma,
                          double *sigma);

#endif // ESTIMATOR_H
