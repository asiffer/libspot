/**
 * @file spot.h
 * @brief Declares Spot structure and methods
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0a
 * @date Fri Mar 10 09:44:55 AM UTC 2023
 * @copyright GNU General Public License version 3
 *
 */

#include "errors.h"
#include "tail.h"

#ifndef SPOT_H
#define SPOT_H

/**
 * @brief
 *
 */
enum SpotResult {
    NORMAL = 0,
    EXCESS = 1,
    ANOMALY = 2,
};

/**
 * @struct Spot
 * @brief Main structure to run the SPOT algorithm
 *
 */
struct Spot {
    double q;                 /**< Probability of an anomaly */
    double level;             /**< Location of the tail */
    int discard_anomalies;    /**< Flag anomalies */
    int low;                  /**< Activate lower tail */
    double __up_down;         /**< internal coefficient */
    double anomaly_threshold; /**< Normal/abnormal threshold */
    double excess_threshold;  /**< Tail threshold */
    unsigned long Nt;         /**< Total number of excesses */
    unsigned long n;          /**< total number of seen data */
    struct Tail tail;         /**< GPD tail */
};

/**
 * @brief Initialize the Spot structure
 *
 * @param spot Spot instance
 * @param q Decision probability (Spot will flag extreme events that will have
 * a probability lower than q)
 * @param low Lower tail mode (0 by defaut for upper tail and 1 for lower tail)
 * @param discard_anomalies Do not include anomalies in the model (default: 1,
 * 0 otherwise)
 * @param level Excess level (it is a high quantile that delimits the tail)
 * @param max_excess Maximum number of data that are kept to analyze the tail
 * @retval 0 OK
 * @retval -ERR_LEVEL_OUT_OF_BOUNDS the level parameter is not between 0 and 1
 * @retval -ERR_Q_OUT_OF_BOUNDS the q parameter is not between 0 and 1-level
 * @retval -ERR_MEMORY_ALLOCATION_FAILED the tail data allocation failed
 */
int spot_init(struct Spot *spot, double q, int low, int discard_anomalies,
              double level, unsigned long max_excess);

/**
 * @brief Free the tail data
 *
 * @param spot Spot instance
 */
void spot_free(struct Spot *spot);

/**
 * @brief Compute the first excess and anomaly thresholds based on training
 * data
 *
 * @param spot Spot instance
 * @param sorted_data Buffer of input data (they must be sorted in ascending
 * order)
 * @param size Size of the buffer
 * @retval 0 OK
 * @retval -ERR_EXCESS_THRESHOLD_IS_NAN the excess threshold is nan
 * @retval -ERR_ANOMALY_THRESHOLD_IS_NA the anomaly threshold is nan
 */
int spot_fit(struct Spot *spot, double *sorted_data, unsigned long size);

/**
 * @brief fit-predict step
 *
 * @param spot Spot instance
 * @param x new value
 * @retval -ERR_DATA_IS_NAN input data is nan
 * @retval NORMAL data is normal
 * @retval EXCESS data lives in the tail
 * @retval ANOMALY data is out of the threshold
 */
int spot_step(struct Spot *spot, double x);

/**
 * @brief Compute the value zq such that P(X>zq) = q
 *
 * @param spot Spot instance
 * @param q Low probability (it must be within the tail)
 * @return the desired quantile
 */
double spot_quantile(struct Spot const *spot, double q);

/**
 * @brief Compute the probability p such that P(X>z) = p
 *
 * @param spot Spot instance
 * @param z High quantile (it must be within the tail)
 * @return the desired probability
 */
double spot_probability(struct Spot const *spot, double z);

#endif // SPOT_H
