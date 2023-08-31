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

// See aakinshin.net/posts/p2-quantile-estimator/
struct P2 {
    double q[5];
    double n[5];
    double np[5];
    double dn[5];
};

static void swap(double *a, double *b) {
    double temp = *a;
    *a = *b;
    *b = temp;
}

static void sort5(double a[static 5]) {
    if (a[1] > a[0]) // Compare 1st and 2nd element #1
        swap(&a[0], &a[1]);
    if (a[3] > a[2]) // Compare 3rd and 4th element #2
        swap(&a[3], &a[2]);
    if (a[0] > a[2]) { // Compare 1st and 3rd element #3
        // run this if 1st element > 3rd element
        swap(&a[1], &a[2]);
        swap(&a[2], &a[3]);
    } else {
        swap(&a[1], &a[2]);
        swap(&a[0], &a[1]);
    }
    // Now 1st, 2nd and 3rd elements are sorted
    // Sort 5th element into 1st, 2nd and 3rd elements
    if (a[4] > a[1]) {     // #4
        if (a[4] > a[0]) { // #5
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
            swap(&a[2], &a[1]);
            swap(&a[1], &a[0]);
        } else {
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
            swap(&a[2], &a[1]);
        }
    } else {
        if (a[4] > a[2]) { // #5
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
        } else {
            swap(&a[4], &a[3]);
        }
    }
    // Sort new 5th element into 2nd, 3rd and 4th
    if (a[4] > a[2]) {     // #6
        if (a[4] > a[1]) { // #7
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
            swap(&a[2], &a[1]);
        } else {
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
        }
    } else {
        if (a[4] > a[3]) { // #7
            swap(&a[4], &a[3]);
        }
    }
}

static void init_p2(struct P2 *p2, double p) {
    for (unsigned i = 0; i < 5; i++) {
        p2->q[i] = 0.0;
        p2->n[i] = (double)i;
        p2->np[i] = 0.0;
        p2->dn[i] = 0.0;
    }

    p2->np[1] = 2 * p;
    p2->np[2] = 4 * p;
    p2->np[3] = 2 + 2 * p;
    p2->np[4] = 4;

    p2->dn[1] = p / 2;
    p2->dn[2] = p;
    p2->dn[3] = (p + 1) / 2;
    p2->dn[4] = 1;
}

static double sign(double d) {
    if (d > 0) {
        return 1;
    }
    if (d < 0) {
        return -1;
    }
    return 0;
}

static double linear(struct P2 *p2, unsigned int i, int d) {
    return p2->q[i] +
           d * (p2->q[i + d] - p2->q[i]) / (p2->n[i + d] - p2->n[i]);
}

static double parabolic(struct P2 *p2, unsigned int i, int d) {
    return p2->q[i] +
           (d / (p2->n[i + 1] - p2->n[i - 1])) *
               ((p2->n[i] - p2->n[i - 1] + d) * (p2->q[i + 1] - p2->q[i]) /
                    (p2->n[i + 1] - p2->n[i]) +
                (p2->n[i + 1] - p2->n[i] - d) * (p2->q[i] - p2->q[i - 1]) /
                    (p2->n[i] - p2->n[i - 1]));
}

static double p2_quantile(struct P2 *p2, double *x, unsigned long size) {
    double xj = 0.0;
    unsigned int k = 0;
    unsigned int i = 0;
    double d = 0.0;
    double qp = 0.0;

    if (size < 5) {
        return 0.0;
    }
    // init q with the 5 first values
    for (i = 0; i < 5; i++) {
        p2->q[i] = x[i];
    }

    sort5(p2->q);
    // now treat the other values
    for (unsigned long j = 5; j < size; j++) {
        xj = x[j];
        if (xj < p2->q[0]) {
            k = 0;
            p2->q[0] = xj;
        } else if (xj > p2->q[4]) {
            k = 3;
            p2->q[4] = xj;
        } else {
            k = 0;
            while (xj > p2->q[k]) {
                k++;
            }
            k--;
            // here q[k] < x < q[k + 1]
            for (i = k + 1; i < 5; i++) {
                p2->n[i] += 1.0;
            }
            for (i = 0; i < 5; i++) {
                p2->np[i] += p2->dn[i];
            }

            // update other markers
            for (i = 1; i < 4; i++) {
                d = p2->np[i] - p2->n[i];
                if ((d >= 1 && (p2->n[i + 1] - p2->n[i]) > 1) ||
                    (d <= -1 && (p2->n[i - 1] - p2->n[i]) < -1)) {
                    d = sign(d);
                    qp = parabolic(p2, i, (int)d);
                    if (!(p2->q[i - 1] < qp && qp < p2->q[i + 1])) {
                        qp = linear(p2, i, (int)d);
                    }
                    p2->q[i] = qp;
                    p2->n[i] += d;
                }
            }
        }
    }
    return p2->q[2];
}

static double quantile(double p, double *data, unsigned long size) {
    struct P2 p2;
    init_p2(&p2, p);
    return p2_quantile(&p2, data, size);
}

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

int spot_fit(struct Spot *spot, double *data, unsigned long size) {
    // total number of excesses
    spot->Nt = 0;
    spot->n = size;

    // compute excess threshold
    double et = _NAN;
    if (spot->low) {
        // take the low quantile (1 - level)
        et = quantile(1. - spot->level, data, size);
    } else {
        et = quantile(spot->level, data, size);
    }
    if (is_nan(et)) {
        return -ERR_EXCESS_THRESHOLD_IS_NAN;
    }
    // here we know that et is not NaN
    spot->excess_threshold = et;

    // fill the tail
    double excess = _NAN;
    for (unsigned long i = 0; i < size; ++i) {
        // positive excess
        excess = spot->__up_down * (data[i] - et);
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
