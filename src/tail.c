/**
 * @file tail.c
 * @brief Implements Tail methods
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b4
 * @date jeu. 17 juil. 2025 08:08:51 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "tail.h"

typedef double (*estimator)(struct Peaks const *, double *, double *);

estimator ESTIMATORS[] = {mom_estimator, grimshaw_estimator};

unsigned int const NB_ESTIMATORS = sizeof(ESTIMATORS) / sizeof(estimator);

int tail_init(struct Tail *tail, unsigned long size) {
    tail->gamma = _NAN;
    tail->sigma = _NAN;
    return peaks_init(&(tail->peaks), size);
}

void tail_free(struct Tail *tail) {
    tail->gamma = _NAN;
    tail->sigma = _NAN;
    // free peaks
    peaks_free(&(tail->peaks));
}

void tail_push(struct Tail *tail, double x) {
    // push the data into the container
    peaks_push(&(tail->peaks), x);
}

double tail_probability(struct Tail const *tail, double s, double d) {
    // d = zq - t
    if (tail->gamma == 0.0) {
        return s * xexp(-d / tail->sigma);
    } else {
        double r = d * (tail->gamma / tail->sigma);
        return s * xpow(1.0 + r, -1.0 / tail->gamma);
    }
}

double tail_quantile(struct Tail const *tail, double s, double q) {
    double r = q / s;
    if (tail->gamma == 0.0) {
        return -tail->sigma * xlog(r);
    }
    return (tail->sigma / tail->gamma) * (xpow(r, -tail->gamma) - 1);
}

double tail_fit(struct Tail *tail) {
    struct Peaks const *peaks = &(tail->peaks);
    double tmp_gamma = _NAN;
    double tmp_sigma = _NAN;

    double max_llhood = _NAN;

    for (unsigned int i = 0; i < NB_ESTIMATORS; ++i) {
        // compare estimators based on their log likelihood
        double llhood = ESTIMATORS[i](peaks, &tmp_gamma, &tmp_sigma);
        if (is_nan(max_llhood) || (llhood > max_llhood)) {
            max_llhood = llhood;
            // update GPD parameters
            tail->gamma = tmp_gamma;
            tail->sigma = tmp_sigma;
        }
    }

    return max_llhood;
}
