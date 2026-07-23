#include "Spot.h"



static double const LOG2 = 0x1.62e42fefa39efp-1;
double const _NAN = 0.0 / 0.0;
double const _INFINITY = +1.0 / 0.0;

#if __SIZEOF_DOUBLE__ == 8

typedef union {
    double d;
    __UINT64_TYPE__ i;
} double_cast;

static double musl_frexp(double x, int *e) {
    double_cast y = {x};

    int ee = y.i >> 52 & 0x7ff;

    if (!ee) {
        if (x) {
            x = musl_frexp(x * 0x1p64, e);
            *e -= 64;
        } else
            *e = 0;
        return x;
    } else if (ee == 0x7ff) {
        return x;
    }

    *e = ee - 0x3fe;
    y.i &= 0x800fffffffffffffull;
    y.i |= 0x3fe0000000000000ull;
    return y.d;
}

static double musl_ldexp(double x, int n) {
    double_cast u = {x};

    // in musl libc, it uses double_t to make aithmetic operations. However it
    // is the same as double once FLT_EVAL_METHOD is 0 (defazult generally) or
    // 1 see https://en.cppreference.com/w/c/numeric/math/float_t double_t y =
    // x;
    double y = x;

    if (n > 1023) {
        y *= 0x1p1023;
        n -= 1023;
        if (n > 1023) {
            y *= 0x1p1023;
            n -= 1023;
            if (n > 1023)
                n = 1023;
        }
    } else if (n < -1022) {
        /* make sure final n < -53 to avoid double
           rounding in the subnormal range */
        y *= 0x1p-1022 * 0x1p53;
        n += 1022 - 53;
        if (n < -1022) {
            y *= 0x1p-1022 * 0x1p53;
            n += 1022 - 53;
            if (n < -1022)
                n = -1022;
        }
    }
    u.i = (__UINT64_TYPE__)(0x3ff + n) << 52;
    x = y * u.d;
    return x;
}

#elif __SIZEOF_DOUBLE__ == 4

typedef union {
    double d;
    __UINT32_TYPE__ i;
} double_cast;

double musl_frexp(double x, int *e) {
    union {
        float f;
        __UINT32_TYPE__ i;
    } y = {x};

    int ee = y.i >> 23 & 0xff;

    if (!ee) {
        if (x) {
            x = frexp(x * 0x1p64, e);
            *e -= 64;
        } else
            *e = 0;
        return x;
    } else if (ee == 0xff) {
        return x;
    }

    *e = ee - 0x7e;
    y.i &= 0x807ffffful;
    y.i |= 0x3f000000ul;
    return y.f;
}

static double musl_ldexp(double x, int n) {
    double_cast u = {x};

    double y = x;
    // float_t y = x;

    if (n > 127) {
        y *= 0x1p127f;
        n -= 127;
        if (n > 127) {
            y *= 0x1p127f;
            n -= 127;
            if (n > 127)
                n = 127;
        }
    } else if (n < -126) {
        y *= 0x1p-126f * 0x1p24f;
        n += 126 - 24;
        if (n < -126) {
            y *= 0x1p-126f * 0x1p24f;
            n += 126 - 24;
            if (n < -126)
                n = -126;
        }
    }
    u.i = (__UINT32_TYPE__)(0x7f + n) << 23;
    x = y * u.f;
    return x;
}

#elif !defined(USE_CUSTOM_FLOAT_UTILS)
#error                                                                        \
    "Cannot provide ldexp/frexp functions: sizeof(double) is neither 8 nor 4."\
    "You must provide your own functions (-DUSE_CUSTOM_FLOAT_UTILS)"
#endif

static frexp_fn libspot_frexp = musl_frexp;

static ldexp_fn libspot_ldexp = musl_ldexp;

void internal_set_float_utils(ldexp_fn l, frexp_fn f) {
    if (l) {
        libspot_ldexp = l;
    }
    if (f) {
        libspot_frexp = f;
    }
}

int is_nan(double x) { return x != x; }

static double _log_cf_11(double z) {
    // could be 4x slower
    double x = z - 1;
    double xx = x + 2;
    double x2 = x * x;

    double xx2 = xx + xx;
    double xx3 = xx + xx2;
    double xx5 = xx3 + xx2;
    double xx7 = xx5 + xx2;
    double xx9 = xx7 + xx2;
    double xx11 = xx9 + xx2;
    double xx13 = xx11 + xx2;
    double xx15 = xx13 + xx2;
    double xx17 = xx15 + xx2;
    double xx19 = xx17 + xx2;
    double xx21 = xx19 + xx2;

    return 2 * x /
           (-x2 / (-4 * x2 /
                       (-9 * x2 /
                            (-16 * x2 /
                                 (-25 * x2 /
                                      (-36 * x2 /
                                           (-49 * x2 /
                                                (-64 * x2 /
                                                     (-81 * x2 /
                                                          (-100 * x2 / xx21 +
                                                           xx19) +
                                                      xx17) +
                                                 xx15) +
                                            xx13) +
                                       xx11) +
                                  xx9) +
                             xx7) +
                        xx5) +
                   xx3) +
            xx);
}

double _exp_cf_6(double z) {
    double z2 = z * z;

    return 2 * z /
               (2 * z2 /
                    (12 * z2 /
                         (60 * z2 / (140 * z2 / (7 * z2 / 11 + 252) + 140) +
                          60) +
                     12) -
                z + 2) +
           1;
}

static double nostdlib_exp(double x) {
    if (is_nan(x)) {
        return _NAN;
    }
    if (x < 0) {
        return 1.0 / nostdlib_exp(-x);
    }
    if (x > LOG2) {
        unsigned int k = x / LOG2;
        double r = x - LOG2 * (double)k;
        return libspot_ldexp(_exp_cf_6(r), k);
        // double_cast w = {.d = _exp_cf_6(r)};
        // w.bits.exponent += k;
        // return w.d;
    }

    return _exp_cf_6(x);
}

static double nostdlib_log(double x) {
    if (x < 0 || is_nan(x)) {
        return _NAN;
    }
    if (x == 0) {
        return -_INFINITY;
    }
    // double_cast casted = {.d = x};
    // if (casted.bits.exponent == 1022 || casted.bits.exponent == 1023) {
    //     // 1/2 < x < 1
    //     // when x is close to 1 (but <1)., it is better
    //     // to call the function directly instead of setting exponent to
    //     // 1023
    //     return _log_cf_11(x);
    // }
    int exponent = 0;
    double mantissa = libspot_frexp(x, &exponent);
    if (exponent == 0 || exponent == -1) {
        return _log_cf_11(x);
    }
    return _log_cf_11(mantissa) + LOG2 * exponent;

    // double exponent = casted.bits.exponent;
    // casted.bits.exponent = 1023;
    // when exponent is 1023, 1 <= casted.d <= 2
    // return logd(casted.d, depth) + (exponent - 1023.0) * LOG2;
    // return _log_cf_11(casted.d) + (exponent - 1023.0) * LOG2;
}

static math_fn libspot_log = nostdlib_log;

static math_fn libspot_exp = nostdlib_exp;

double xlog(double x) { return libspot_log(x); }

double xexp(double x) { return libspot_exp(x); }


static double nostdlib_pow(double a, double x) { return xexp(x * xlog(a)); }

static math2_fn libspot_pow = nostdlib_pow;

void internal_set_math_functions(math_fn lo, math_fn ex, math2_fn po) {
    if (lo) {
        libspot_log = lo;
    }
    if (ex) {
        libspot_exp = ex;
    }
    if (po) {
        libspot_pow = po;
    }
}

double xpow(double a, double x) { return libspot_pow(a, x); }

double xmin(double a, double b) {
    if (is_nan(a) || is_nan(b)) {
        return _NAN;
    }
    if (a < b) {
        return a;
    }
    return b;
}


double const BRENT_DEFAULT_EPSILON = 2.0e-8;

unsigned long const BRENT_ITMAX = 200;


static double _fabs(double a) {
    if (a < 0) {
        return -a;
    }
    return a;
}

double brent(int *found, double x1, double x2, real_function func, void *extra,
             double tol) {
    // we assume we found the root
    // it will be set to zero in error cases
    *found = 1;

    double a = x1;
    double b = x2;
    double c = x2;
    double d = 0.0;
    double e = 0.0;
    // double min1 = 0.0;
    // double min2 = 0.0;

    double fa = func(a, extra);
    double fb = func(b, extra);
    double fc = 0.0;
    // double p = 0.0;
    // double q = 0.0;
    // double r = 0.0;
    // double s = 0.0;
    // double tol1 = 0.0;
    // double xm = 0.0;

    if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0)) {
        *found = 0;
        return 0.0;
    }

    fc = fb;
    for (unsigned long iter = 0; iter < BRENT_ITMAX; iter++) {
        if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
            c = a; // Rename a, b, c and adjust bounding interval
            fc = fa;
            e = d = b - a;
        }
        if (_fabs(fc) < _fabs(fb)) {
            a = b;
            b = c;
            c = a;
            fa = fb;
            fb = fc;
            fc = fa;
        }
        double tol1 = 2.0 * BRENT_DEFAULT_EPSILON * _fabs(b) +
                      0.5 * tol; // Convergence check.
        double xm = 0.5 * (c - b);
        if (_fabs(xm) <= tol1 || fb == 0.0)
            return b;
        if (_fabs(e) >= tol1 && _fabs(fa) > _fabs(fb)) {
            double s = fb / fa; // Attempt inverse quadratic interpolation.
            double p;
            double q;

            if (a == c) {
                p = 2.0 * xm * s;
                q = 1.0 - s;
            } else {
                q = fa / fc;
                double r = fb / fc;
                p = s * (2.0 * xm * q * (q - r) - (b - a) * (r - 1.0));
                q = (q - 1.0) * (r - 1.0) * (s - 1.0);
            }
            if (p > 0.0)
                q = -q; // Check whether in bounds.
            p = _fabs(p);
            double min1 = 3.0 * xm * q - _fabs(tol1 * q);
            double min2 = _fabs(e * q);
            if (2.0 * p < (min1 < min2 ? min1 : min2)) {
                e = d; // Accept interpolation.
                d = p / q;
            } else {
                d = xm; // Interpolation failed, use bisection.
                e = d;
            }
        } else { // Bounds decreasing too slowly, use bisection.
            d = xm;
            e = d;
        }
        a = b; // Move last best guess to a.
        fa = fb;
        if (_fabs(d) > tol1) // Evaluate new trial root.
            b += d;
        else
            b += ((xm) >= 0.0 ? _fabs(tol1) : -_fabs(tol1));
        fb = func(b, extra);
    }
    // Maximum number of iterations exceeded
    *found = 0;
    return 0.0;
}


int ubend_init(struct Ubend *ubend, double *buffer, unsigned long capacity) {
    ubend->cursor = 0;
    ubend->filled = 0;
    ubend->capacity = capacity;
    ubend->last_erased_data = _NAN;
    ubend->data = buffer;
    return 0;
}

unsigned long ubend_size(struct Ubend const *ubend) {
    if (ubend->filled) {
        return ubend->capacity;
    }
    return ubend->cursor;
}

double ubend_push(struct Ubend *ubend, double x) {
    // if the container has already been filled
    // we must keep in memory the data we will
    // erase
    if (ubend->filled) {
        ubend->last_erased_data = ubend->data[ubend->cursor];
    }

    // assign to value at cursor
    ubend->data[ubend->cursor] = x;

    // increment cursor
    if (ubend->cursor == (ubend->capacity - 1)) {
        ubend->cursor = 0;
        ubend->filled = 1;
    } else {
        ubend->cursor += 1;
    }

    return ubend->last_erased_data;
}



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


void sort5(double a[5]) {
    if (a[1] < a[0]) // Compare 1st and 2nd element #1
        swap(&a[0], &a[1]);
    if (a[3] < a[2]) // Compare 3rd and 4th element #2
        swap(&a[3], &a[2]);
    if (a[0] < a[2]) { // Compare 1st and 3rd element #3
        // run this if 1st element < 3rd element
        swap(&a[1], &a[2]);
        swap(&a[2], &a[3]);
    } else {
        swap(&a[1], &a[2]);
        swap(&a[0], &a[1]);
    }
    // Now 1st, 2nd and 3rd elements are sorted
    // Sort 5th element into 1st, 2nd and 3rd elements
    if (a[4] < a[1]) {     // #4
        if (a[4] < a[0]) { // #5
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
        if (a[4] < a[2]) { // #5
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
        } else {
            swap(&a[4], &a[3]);
        }
    }
    // Sort new 5th element into 2nd, 3rd and 4th
    if (a[4] < a[2]) {     // #6
        if (a[4] < a[1]) { // #7
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
            swap(&a[2], &a[1]);
        } else {
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
        }
    } else {
        if (a[4] < a[3]) { // #7
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
        return 1.0;
    }
    if (d < 0) {
        return -1.0;
    }
    return 0.0;
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

static double quantile(struct P2 *p2, double const *x, unsigned long size) {
    unsigned int k;
    unsigned int i;
    double qp;

    if (size < 5) {
        return _NAN;
    }
    // init q with the 5 first values
    for (i = 0; i < 5; i++) {
        p2->q[i] = x[i];
    }

    sort5(p2->q);
    // now treat the other values
    for (unsigned long j = 5; j < size; j++) {
        double xj = x[j];
        if (xj < p2->q[0]) {
            // k = 0;
            p2->q[0] = xj;
        } else if (xj > p2->q[4]) {
            // k = 3;
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
                double d = p2->np[i] - p2->n[i];
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

double p2_quantile(double p, double const *data, unsigned long size) {
    struct P2 p2;
    init_p2(&p2, p);
    return quantile(&p2, data, size);
}


int peaks_init(struct Peaks *peaks, double *buffer, unsigned long size) {
    peaks->e = 0.0;
    peaks->e2 = 0.0;
    peaks->min = _NAN;
    peaks->max = _NAN;
    return ubend_init(&peaks->container, buffer, size);
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
        peaks->e2 += value * value;
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


typedef double (*estimator)(struct Peaks const *, double *, double *);

estimator ESTIMATORS[] = {mom_estimator, grimshaw_estimator};

unsigned int const NB_ESTIMATORS = sizeof(ESTIMATORS) / sizeof(estimator);

int tail_init(struct Tail *tail, double *buffer, unsigned long size) {
    tail->gamma = _NAN;
    tail->sigma = _NAN;
    return peaks_init(&(tail->peaks), buffer, size);
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


#ifndef VERSION
#define VERSION "UNDEFINED"
#endif

#ifndef LICENSE
#define LICENSE "LGPL-3.0-or-later"
#endif


static const char *version = VERSION;
static const char *license = LICENSE;

int spot_init(struct Spot *spot, double q, int low, int discard_anomalies,
              double level, double *buffer, unsigned long max_excess) {
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

    // always return 0
    tail_init(&(spot->tail), buffer, max_excess);

    spot->anomaly_threshold = _NAN;
    spot->excess_threshold = _NAN;

    return 0;
}

void spot_reset(struct Spot *spot) {
    // reset counters
    spot->Nt = 0;
    spot->n = 0;
    // retrieve the buffer
    double *buffer = spot->tail.peaks.container.data;
    unsigned long size = spot->tail.peaks.container.capacity;
    // reinitialize tail with the same buffer
    tail_init(&(spot->tail), buffer, size);

    spot->anomaly_threshold = _NAN;
    spot->excess_threshold = _NAN;
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



static char *strncpy(char *dst, const char *src, unsigned long size) {
    if (size == 0) {
        return dst;
    }
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

void set_math_functions(math_fn lo, math_fn ex, math2_fn po) {
    internal_set_math_functions(lo, ex, po);
}


static const char *errors[] = {
    "Memory allocation failed", // ERR_MEMORY_ALLOCATION_FAILED
    "The level parameter is out of bounds (it must be between 0 and 1, but close to 1)", // ERR_LEVEL_OUT_OF_BOUNDS
    "The q parameter must between 0 and 1-level", // ERR_Q_OUT_OF_BOUNDS
    "The excess threshold has not been initialized", // ERR_EXCESS_THRESHOLD_IS_NAN
    "The anomaly threshold has not been initialized", // ERR_ANOMALY_THRESHOLD_IS_NAN
    "The input data is NaN",                          // ERR_DATA_IS_NAN
}; // clang-format on

void libspot_error(enum LibspotError err, char *buffer, unsigned long size) {
    if ((err >= ERR_MEMORY_ALLOCATION_FAILED) && (err <= ERR_DATA_IS_NAN)) {
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
