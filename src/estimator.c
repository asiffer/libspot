/**
 * @file estimator.c
 * @brief Implements GPD parameters estimators
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b4
 * @date jeu. 17 juil. 2025 08:08:51 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "estimator.h"

double mom_estimator(struct Peaks const *peaks, double *gamma, double *sigma) {
    const double E = peaks_mean(peaks);
    const double V = peaks_var(peaks);
    const double R = E * E / V;

    *gamma = 0.5 * (1.0 - R);
    *sigma = 0.5 * E * (1.0 + R);
    return log_likelihood(peaks, *gamma, *sigma);
}

static double grimshaw_w(double x, void *peaks_as_void) {
    struct Peaks *peaks = (struct Peaks *)peaks_as_void;
    unsigned long Nt_local = peaks_size(peaks);
    double u = 0.0;
    double v = 0.0;

    for (unsigned long i = 0; i < Nt_local; ++i) {
        double s = 1. + x * peaks->container.data[i];
        u += 1 / s;
        v += xlog(s);
    }
    return (u / Nt_local) * (1.0 + v / Nt_local) - 1.0;
}

static double grimshaw_v(double x, struct Peaks const *peaks) {
    double v = 0.0;
    unsigned long Nt_local = peaks_size(peaks);
    for (unsigned long i = 0; i < Nt_local; ++i) {
        v += xlog(1.0 + x * peaks->container.data[i]);
    }
    return 1.0 + v / Nt_local;
}

static double grimshaw_simplified_log_likelihood(double x_star,
                                                 struct Peaks const *peaks,
                                                 double *gamma,
                                                 double *sigma) {
    if (x_star == 0) {
        *gamma = 0.0;
        *sigma = peaks_mean(peaks);
    } else {
        *gamma = grimshaw_v(x_star, peaks) - 1;
        *sigma = *gamma / x_star;
    }
    return log_likelihood(peaks, *gamma, *sigma);
}

double grimshaw_estimator(struct Peaks const *peaks, double *gamma,
                          double *sigma) {
    double mini = peaks->min;
    double maxi = peaks->max;
    double mean = peaks_mean(peaks);

    double epsilon = xmin(BRENT_DEFAULT_EPSILON, 0.5 / maxi);
    double r;
    double a, b;

    int found[] = {1, 0, 0};       // true, false, false
    double roots[] = {0., 0., 0.}; // 0., ?, ?
    unsigned int const left = 1;   // left root index
    unsigned int const right = 2;  // right root index

    double tmp_gamma, tmp_sigma;
    double llhood, max_llhood;
    // left root -------------------------------------------------------------
    a = -1.0 / maxi + epsilon;
    b = -epsilon;
    r = brent(&found[left], a, b, grimshaw_w, (void *)peaks,
              BRENT_DEFAULT_EPSILON);
    if (found[left]) {
        roots[left] = r;
    }

    // right root -------------------------------------------------------------
    a = epsilon;
    b = 2.0 * (mean - mini) / (mini * mini);
    r = brent(&found[right], a, b, grimshaw_w, (void *)peaks,
              BRENT_DEFAULT_EPSILON);
    if (found[right]) {
        roots[right] = r;
    }

    // compare all roots
    // first start with zero (it also assign gamma and sigma)
    max_llhood =
        grimshaw_simplified_log_likelihood(roots[0], peaks, gamma, sigma);
    // now check for the roots
    for (unsigned int k = 1; k < 3; ++k) {
        // if a root has been found
        if (found[k]) {
            // compute the log likelihood
            llhood = grimshaw_simplified_log_likelihood(
                roots[k], peaks, &tmp_gamma, &tmp_sigma);
            // assign values if we have a better result
            if (llhood > max_llhood) {
                max_llhood = llhood;
                *gamma = tmp_gamma;
                *sigma = tmp_sigma;
            }
        }
    }
    return max_llhood;
}
