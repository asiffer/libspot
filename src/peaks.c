/**
 * @file peaks.c
 * @brief Implements Peaks methods
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b4
 * @date jeu. 17 juil. 2025 08:08:51 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "peaks.h"

int peaks_init(struct Peaks *peaks, unsigned long size) {
    peaks->e = 0.0;
    peaks->e2 = 0.0;
    peaks->min = _NAN;
    peaks->max = _NAN;
    return ubend_init(&peaks->container, size);
}

void peaks_free(struct Peaks *peaks) {
    peaks->e = _NAN;
    peaks->e2 = _NAN;
    peaks->min = _NAN;
    peaks->max = _NAN;
    // free container
    ubend_free(&(peaks->container));
}

static unsigned long peaks_update_stats(struct Peaks *peaks) {
    unsigned long max_iteration;

    /* reset min and max */
    peaks->min = _NAN;
    peaks->max = _NAN;
    /* reset accumulators */
    peaks->e = 0.0;
    peaks->e2 = 0.0;

    max_iteration = ubend_size(&(peaks->container));

    for (unsigned long i = 0; i < max_iteration; ++i) {
        // value = *iterator;
        double value = peaks->container.data[i];
        peaks->e += value;
        peaks->e2 = value * value;
        if (is_nan(peaks->min) || (value < peaks->min)) {
            peaks->min = value;
        }
        if (is_nan(peaks->max) || (value > peaks->max)) {
            peaks->max = value;
        }
    }
    return max_iteration;
}

unsigned long peaks_size(struct Peaks const *peaks) {
    return ubend_size(&(peaks->container));
}

void peaks_push(struct Peaks *peaks, double x) {
    double const erased = ubend_push(&(peaks->container), x);
    unsigned long const size = peaks_size(peaks);
    /* increment the stats */
    peaks->e += x;
    peaks->e2 += x * x;

    // first we update the stats with the value of x
    if ((size == 1) || (x < peaks->min)) {
        peaks->min = x;
    }
    if ((size == 1) || (x > peaks->max)) {
        peaks->max = x;
    }

    // then we treat the case where a data has been erased
    // In this case we must update the accumulators and possibly
    // update the min/max
    if (!is_nan(erased)) {
        peaks->e -= erased;
        peaks->e2 -= erased * erased;
        if ((erased <= peaks->min) || (erased >= peaks->max)) {
            /* here we have to loop in the container to ensure having
            the right stats (in particular min and max). However, we
            also update e and e2 (the in/decrements may create precision
            errors) */
            peaks_update_stats(peaks);
        }
    }
}

double peaks_mean(struct Peaks const *peaks) {
    return peaks->e / (double)peaks_size(peaks);
}

double peaks_var(struct Peaks const *peaks) {
    double const size = (double)peaks_size(peaks);
    double mean = peaks->e / size;
    return (peaks->e2 / size) - (mean * mean);
}

double log_likelihood(struct Peaks const *peaks, double gamma, double sigma) {
    unsigned long Nt_local = ubend_size(&(peaks->container));
    double Nt = (double)Nt_local;
    if (gamma == 0.) {
        return -Nt * xlog(sigma) - peaks->e / sigma;
    }

    double r = -Nt * xlog(sigma);
    const double c = 1. + 1. / gamma;
    const double x = gamma / sigma;
    for (unsigned long i = 0; i < Nt_local; ++i) {
        r += -c * xlog(1 + x * peaks->container.data[i]);
    }
    return r;
}
