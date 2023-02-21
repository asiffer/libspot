/**
 * @file xmath.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-09-26
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * @brief LOG_CF_DEPTH is a parameter to modify the precision of the
 * logarithm
 * @details To test the impact of the parameter, you can use the
 * following oneliner:
rm -f /tmp/results; for i in {8..15}; do make clean && make -s test \
CMOREFLAGS="-DLOG_CF_DEPTH=$i" && cat build/results/xmath_test.txt|grep \
'test_log_estimate_precision:INFO' >> /tmp/results; done; echo "[$(cat \
/tmp/results|awk -F 'INFO: ' '{print $2}'|tr '\n' ','|sed 's/,$//g')]"
 */
#ifndef LOG_CF_DEPTH
#define LOG_CF_DEPTH 18
#endif

/**
 * @brief EXP_CF_DEPTH is a parameter to modify the precision of the
 * exponential
 * @details The following precisions have been noticed on the
 * interval [-19, 11]. The big values are problematic.
 * @details To test the impact of the parameter, you can use the
 * following oneliner:
rm -f /tmp/results; for i in {8..15}; do make clean && make -s test \
CMOREFLAGS="-DEXP_CF_DEPTH=$i" && cat build/results/xmath_test.txt|grep \
'test_exp_estimate_precision:INFO' >> /tmp/results; done; echo "[$(cat \
/tmp/results|awk -F 'INFO: ' '{print $2}'|tr '\n' ','|sed 's/,$//g')]"
 */
#ifndef EXP_CF_DEPTH
#define EXP_CF_DEPTH 22
#endif

extern double const _NAN;
extern double const _INFINITY;

#ifndef XMATH_H
#define XMATH_H

/**
 * @brief Compute natural logarithm with Shank's algorithm
 * @details It returns -oo for x=0 and NaN for x<0
 * @param x input value
 * @return double
 */
double log(double x);

/**
 * @brief Compute exponential with continuous fraction
 * @details It uses the formula of Khovanskii:
 * Khovanskii, A. N. (1963). The application of continued fractions and their
 * generalizations to problems in approximation theory (p. 144). Groningen:
 * Noordhoff.
 * Formula (5.5) p.114
 * @param x
 * @return double
 */
double exp(double x);

/**
 * @brief Compute a^x with log and exp :)
 *
 * @param a
 * @param x
 * @return double
 */
double pow(double a, double x);

/**
 * @brief Return the minimum of two values
 *
 * @param a
 * @param b
 * @return double
 */
double min(double a, double b);

/**
 * @brief Check if a double is NAN
 *
 * @param x input value to check
 * @return 1 when x is nan
 * @return 0 otherwise
 */
int is_nan(double x);

/**
 * @brief
 *
 */
// int is_inf(double x);
#endif // XMATH_H
