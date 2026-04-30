/**
 * @file estimator.h
 * @brief Declares GPD parameters estimators
 * @author Alban Siffer (31479857+asiffer@users.noreply.github.com)
 * @version 3.0.0
 * @date mar. 14 avril 2026 14:59:11 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */

#ifndef ESTIMATOR_H
#define ESTIMATOR_H

#include "peaks.h"

/**
 * @brief Method of moments estimator for GPD parameters
 *
 * @param peaks Peaks instance
 * @param[out] gamma computed GPD gamma parameter
 * @param[out] sigma computed GPD sigma parameter
 * @return the log-likelihood of the estimation
 */
double mom_estimator(struct Peaks const *peaks, double *gamma, double *sigma);

/**
 * @brief Grimshaw estimator for GPD parameters
 *
 * @param peaks Peaks instance
 * @param[out] gamma computed GPD gamma parameter
 * @param[out] sigma computed GPD sigma parameter
 * @return the log-likelihood of the estimation
 */
double grimshaw_estimator(struct Peaks const *peaks, double *gamma,
                          double *sigma);

#endif // ESTIMATOR_H
