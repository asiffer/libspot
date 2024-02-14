/**
 * @file spot.c
 * @brief Implements Spot methods
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "spot.h"

#ifndef VERSION
#define VERSION "UNDEFINED"
#endif

#ifndef LICENSE
#define LICENSE "LGPL-3.0-or-later"
#endif

// store version and license in strings
static const char *version = VERSION;
static const char *license = LICENSE;

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

int spot_fit(struct Spot *spot, double const *data, unsigned long size) {
    // total number of excesses
    spot->Nt = 0;
    spot->n = size;

    // compute excess threshold
    double et;
    if (spot->low) {
        // take the low quantile (1 - level)
        et = p2_quantile(1. - spot->level, data, size);
    } else {
        et = p2_quantile(spot->level, data, size);
    }
    if (is_nan(et)) {
        return -ERR_EXCESS_THRESHOLD_IS_NAN;
    }
    // here we know that et is not NaN
    spot->excess_threshold = et;

    // fill the tail
    for (unsigned long i = 0; i < size; ++i) {
        // positive excess
        double excess = spot->__up_down * (data[i] - et);
        if (excess > 0) {
            // it is a real excess
            spot->Nt++;
            tail_push(&(spot->tail), excess);
        }
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
        // update threshold
        spot->anomaly_threshold = spot_quantile(spot, spot->q);
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

// Extra functions -----------------------------------------------------------

void set_allocators(malloc_fn m, free_fn f) { internal_set_allocators(m, f); }

/**
 * @brief Copy at most size-1 byte from src to dst (fill dst with zeros until
 * size-1)
 *
 * @param dst output buffer
 * @param src input buffer
 * @param size size of the output buffer
 * @return char* pointer to the output buffer
 */
static char *strncpy(char *dst, const char *src, unsigned long size) {
    if (dst) {
        unsigned long i = 0;
        for (; (i < size) && (src[i] != '\0'); i++) {
            dst[i] = src[i];
        }
        for (; i < size; i++) {
            dst[i] = '\0';
        }
        dst[size - 1] = '\0';
    }
    return dst;
}

void set_float_utils(ldexp_fn l, frexp_fn f) {
    internal_set_float_utils(l, f);
}

// clang-format off
static const char *errors[] = {
    "Memory allocation failed", // ERR_MEMORY_ALLOCATION_FAILED
    "The level parameter is out of bounds (it must be between 0 and 1, but close to 1)", // ERR_LEVEL_OUT_OF_BOUNDS
    "The q parameter must between 0 and 1-level", // ERR_Q_OUT_OF_BOUNDS
    "The excess threshold has not been initialized", // ERR_EXCESS_THRESHOLD_IS_NAN
    "The anomaly threshold has not been initialized", // ERR_ANOMALY_THRESHOLD_IS_NAN
    "The input data is NaN",                          // ERR_DATA_IS_NAN
}; // clang-format on

void libspot_error(enum LibspotError err, char *buffer, unsigned long size) {
    if ((err >= ERR_MEMORY_ALLOCATION_FAILED) && (err < ERR_DATA_IS_NAN)) {
        int index = err - ERR_MEMORY_ALLOCATION_FAILED;
        strncpy(buffer, errors[index], size);
    }
}

void libspot_version(char *buffer, unsigned long size) {
    strncpy(buffer, version, size);
}

void libspot_license(char *buffer, unsigned long size) {
    strncpy(buffer, license, size);
}
