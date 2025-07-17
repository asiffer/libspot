/**
 * @file brent.c
 * @brief Implements Brent's method
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b4
 * @date jeu. 17 juil. 2025 08:08:51 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "brent.h"

double const BRENT_DEFAULT_EPSILON = 2.0e-8;

unsigned long const BRENT_ITMAX = 200;

// fabs is reserved on windows
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
