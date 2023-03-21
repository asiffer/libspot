/**
 * @file spot.c
 * @brief Implements Spot methods
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU General Public License version 3
 *
 */
#include "spot.h"

// static inline double __up_down(struct Spot *spot) {
//     return 2.0 * (1.0 - spot->low) - 1.0;
// };

int spot_init(struct Spot *spot, double q, int low, int discard_anomalies,
              double level, unsigned long max_excess) {
    if ((level < 0.) || (level >= 1.)) {
        return -ERR_LEVEL_OUT_OF_BOUNDS;
    }
    if ((q >= (1. - level)) || (q <= 0.0)) {
        return -ERR_Q_OUT_OF_BOUNDS;
    }
    spot->q = q;
    spot->level = level;

    if (low) {
        spot->low = 1;
        spot->__up_down = -1.0;
    } else {
        spot->low = 0;
        spot->__up_down = 1.0;
    }

    if (discard_anomalies) {
        spot->discard_anomalies = 1;
    } else {
        spot->discard_anomalies = 0;
    }

    // no values yet
    spot->n = 0;
    spot->Nt = 0;

    // in all cases, tail is init to
    // ensure the struct is fully initialized
    if (tail_init(&(spot->tail), max_excess) < 0) {
        return -ERR_MEMORY_ALLOCATION_FAILED;
    }

    spot->anomaly_threshold = _NAN;
    spot->excess_threshold = _NAN;

    return 0;
}

void spot_free(struct Spot *spot) {
    // put everything to NAN or
    spot->q = _NAN;
    spot->level = _NAN;
    spot->discard_anomalies = -1;
    spot->low = -1;
    spot->__up_down = _NAN;
    spot->anomaly_threshold = _NAN;
    spot->excess_threshold = _NAN;
    spot->Nt = 0;
    spot->n = 0;
    // free tail
    tail_free(&(spot->tail));
}

int spot_fit(struct Spot *spot, double *sorted_data, unsigned long size) {

    unsigned long rank = (unsigned long)(spot->level * (double)size);
    unsigned long i = rank;
    unsigned long i_max = size;

    // modify the values if its is a low tail
    if (spot->low) {
        rank = size - rank;
        i = 0;
        i_max = rank;
    }

    // total number of excesses
    spot->Nt = i_max - i;
    spot->n = size;

    // maybe here we can ensure that rank is not out of bounds
    spot->excess_threshold = sorted_data[rank];
    if (is_nan(spot->excess_threshold)) {
        return -ERR_EXCESS_THRESHOLD_IS_NAN;
    }

    // initialization of the  excesses
    for (; i < i_max; ++i) {
        // excesses must be positive
        // up_down ensure the values are positive
        tail_push(&(spot->tail),
                  spot->__up_down * (sorted_data[i] - spot->excess_threshold));
    }

    // fit with the pushed data
    tail_fit(&(spot->tail));

    // compute a first anomaly threshold
    spot->anomaly_threshold = spot_quantile(spot, spot->q);
    if (is_nan(spot->anomaly_threshold)) {
        return -ERR_ANOMALY_THRESHOLD_IS_NAN;
    }

    return 0;
}

int spot_step(struct Spot *spot, double x) {
    if (is_nan(x)) {
        return -ERR_DATA_IS_NAN;
    }

    if ((spot->discard_anomalies) &&
        (spot->__up_down * (x - spot->anomaly_threshold) > 0)) {
        return ANOMALY;
    }

    // increment number of data (without the anomalies)
    spot->n++;

    double ex = spot->__up_down * (x - spot->excess_threshold);
    if (ex >= 0.0) {
        // increment number of excesses
        spot->Nt++;
        tail_push(&(spot->tail), ex);
        tail_fit(&(spot->tail));
        return EXCESS;
    }

    return NORMAL;
}

double spot_quantile(struct Spot const *spot, double q) {
    double s = (double)(spot->Nt) / (double)(spot->n);
    return spot->excess_threshold +
           spot->__up_down * tail_quantile(&spot->tail, s, q);
}

double spot_probability(struct Spot const *spot, double z) {
    double s = (double)(spot->Nt) / (double)(spot->n);
    return tail_probability(&(spot->tail), s,
                            spot->__up_down * (z - spot->excess_threshold));
}
