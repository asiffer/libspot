/**
 * @file spot.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "tail.h"

#ifndef SPOT_H
#define SPOT_H

enum {
    ERR_MEMORY_ALLOCATION_FAILED = 1000,
    ERR_LEVEL_OUT_OF_BOUNDS,
    ERR_Q_OUT_OF_BOUNDS,
    ERR_EXCESS_THRESHOLD_IS_NAN,
    ERR_ANOMALY_THRESHOLD_IS_NAN,
    ERR_DATA_IS_NAN,
};

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
 * @brief
 *
 */
struct Spot {
    double q;
    double level;
    int discard_anomalies;
    int low;
    double __up_down;         // internal coefficient
    double anomaly_threshold; //
    double excess_threshold;  //
    unsigned long Nt;         // Total number of excesses
    unsigned long n;          // total number of seen data
    struct Tail tail;         // GPD tail
};

/**
 * @brief
 *
 * @param spot Spot instance
 * @param q Decision probability (Spot will flag extreme events that will have
 * a probability lower than q)
 * @param low Lower tail mode (0 by defaut for upper tail and 1 for lower tail)
 * @param discard_anomalies Do not include anomalies in the model (default: 1,
 * 0 otherwise)
 * @param level Excess level (it is a high quantile that delimits the tail)
 * @param max_excess Maximum number of data that are kept to analyze the tail
 * @return int
 */
int spot_init(struct Spot *spot, double q, int low, int discard_anomalies,
              double level, unsigned long max_excess);

/**
 * @brief
 *
 * @param spot
 */
void spot_free(struct Spot *spot);

/**
 * @brief
 *
 * @param spot Spot instance
 * @param sorted_data Buffer of input data (they must be sorted in ascending
 * order)
 * @param size Size of the buffer
 */
int spot_fit(struct Spot *spot, double *sorted_data, unsigned long size);

/**
 * @brief
 *
 * @param spot Spot instance
 * @param x new value
 * @return int Returns NORMAL, EXCESS or ANOMALY
 */
int spot_step(struct Spot *spot, double x);

/**
 * @brief
 *
 * @param spot Spot instance
 * @param q Low probability (it must be within the tail)
 * @return double
 */
double spot_quantile(struct Spot const *spot, double q);

/**
 * @brief
 *
 * @param spot Spot instance
 * @param z High quantile (it must be within the tail)
 * @return double
 */
double spot_probability(struct Spot const *spot, double z);

#endif // SPOT_H
