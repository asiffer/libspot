/**
 * @file brent.c
 * @brief Implements Brent's method
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0a
 * @date Fri Mar 10 09:44:55 AM UTC 2023
 * @copyright GNU General Public License version 3
 *
 */
#include "brent.h"

double const BRENT_DEFAULT_EPSILON = 0.0000001;

static double fabs(double a) {
    if (a < 0) {
        return -a;
    }
    return a;
}

static void swap(double *x, double *y) {
    double tmp = *x;
    *x = *y;
    *y = tmp;
}

// https://handwiki.org/wiki/Brent%27s_method
double brent(int *found, double a, double b, real_function f, void *extra,
             double epsilon) {
    double fa = f(a, extra);
    double fb = f(b, extra);
    if (fa * fb >= 0) {
        *found = 0;
        return 0.;
    }

    if (fabs(fa) < fabs(fb)) { // swap (a,b)
        swap(&a, &b);
        swap(&fa, &fb);
    }

    double c = a;
    double fc = fa;
    int mflag = 1;
    double s = 0.0;
    double fs = 0.0;
    double t = 0.0;
    int cond1, cond2, cond3 = 0;
    double d = 0.0;

    while ((fb != 0.0) && fabs(b - a) > epsilon) {
        if ((fa != fc) && (fb != fc)) {
            s = (a * fb * fc) / ((fa - fb) * (fa - fc)) +
                (b * fa * fc) / ((fb - fa) * (fb - fc)) +
                (c * fa * fb) / ((fc - fa) * (fc - fb));
        } else {
            s = b - fb * (b - a) / (fb - fa);
        }

        t = (3 * a + b) / 4;
        cond1 = !(((t < s) && (s < b)) || ((b < s) && (s < t)));
        cond2 = (mflag == 1) && (fabs(s - b) >= 0.5 * fabs(b - c));
        cond3 = (mflag == 0) && (fabs(s - b) >= 0.5 * fabs(d - c));
        if (cond1 || cond2 || cond3) {
            s = (a + b) / 2.0;
            mflag = 1;
        } else {
            mflag = 0;
        }

        fs = f(s, extra);
        d = c; // first time d is set

        c = b;
        fc = fb;
        if (fa * fs < 0) {
            b = s;
            fb = fs;
        } else {
            a = s;
            fa = fs;
        }

        if (fabs(fa) < fabs(fb)) { // swap (a,b)
            swap(&a, &b);
            swap(&fa, &fb);
        }
    }

    *found = 1;
    return b;
}
