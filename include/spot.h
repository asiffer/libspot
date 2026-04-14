/**
 * @file spot.h
 * @brief Declares Spot structure and methods
 * @author Alban Siffer (31479857+asiffer@users.noreply.github.com)
 * @version 3.0a0
 * @date mar. 14 avril 2026 14:59:11 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */

#ifndef SPOT_H
#define SPOT_H

#include "p2.h"
#include "tail.h"

// Spot API ------------------------------------------------------------------

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
 * @param buffer Buffer to store all the excesses (tail data)
 * @param max_excess Size of the buffer (number of elements)
 * @retval 0 OK
 * @retval -ERR_LEVEL_OUT_OF_BOUNDS the level parameter is not between 0 and 1
 * @retval -ERR_Q_OUT_OF_BOUNDS the q parameter is not between 0 and 1-level
 * @retval -ERR_MEMORY_ALLOCATION_FAILED the tail data allocation failed
 */
int spot_init(struct Spot *spot, double q, int low, int discard_anomalies,
              double level, double *buffer, unsigned long max_excess);

/**
 * @brief Reset the internal structure of the Spot instance. It reuses the same
 * backing buffer.
 *
 * @param spot Spot instance
 */
void spot_reset(struct Spot *spot);

/**
 * @brief Compute the first excess and anomaly thresholds based on training
 * data
 *
 * @param spot Spot instance
 * @param data Buffer of input data
 * @param size Size of the buffer
 * @retval 0 OK
 * @retval -ERR_EXCESS_THRESHOLD_IS_NAN the excess threshold is nan
 * @retval -ERR_ANOMALY_THRESHOLD_IS_NA the anomaly threshold is nan
 */
int spot_fit(struct Spot *spot, double const *data, unsigned long size);

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

/* Extra functions */

/**
 * @brief Set the ldexp/frexp functions
 *
 * By default these functions are provided but the API
 * allows to change them. It is low level, you might rather
 * use set_math_functions to provide optimized math functions.
 *
 * @param l pointer to a `ldexp` function (`NULL` if you do not want to change
 * it)
 * @param f pointer to a `frexp` function (`NULL` if you do not want to change
 * it)
 */
void set_float_utils(ldexp_fn l, frexp_fn f);

/**
 * @brief Set the log, exp and pow functions. It is recommended when you have
 * access to optimized versions (e.g., from the standard library).
 *
 * @param lo pointer to a `log` function (`NULL` if you do not want to change
 * it)
 * @param ex pointer to a `exp` function (`NULL` if you do not want to change
 * it)
 * @param po pointer to a `pow` function (`NULL` if you do not want to change
 * it)
 */
void set_math_functions(math_fn lo, math_fn ex, math2_fn po);

/**
 * @brief Return the version of libspot
 *
 * @param[out] buffer input buffer to fill with
 * @param size size of the input buffer
 */
void libspot_version(char *buffer, unsigned long size);

/**
 * @brief Return the license of the library
 *
 * @param[out] buffer input buffer to fill with
 * @param size size of the input buffer
 */
void libspot_license(char *buffer, unsigned long size);

/**
 * @brief Return a string related to an error code
 *
 * @param err error code
 * @param[out] buffer input buffer to fill with
 * @param size size of the input buffer
 */
void libspot_error(enum LibspotError err, char *buffer, unsigned long size);

#endif // SPOT_H
