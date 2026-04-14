/**
 * @file ubend.h
 * @brief Declares Ubend structure and methods
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b4
 * @date jeu. 17 juil. 2025 08:08:51 UTC
 * @copyright GNU Lesser General Public License v3.0
 */

#include "xmath.h"

#ifndef UBEND_H
#define UBEND_H

/**
 * @brief Ubend structure initializer
 *
 * @param ubend structure to init
 * @param buffer buffer to store the data
 * @param capacity size of the buffer (number of elements)
 * @return 0 always returns 0
 */
int ubend_init(struct Ubend *ubend, double *buffer, unsigned long capacity);

/**
 * @brief Return the current size of the container
 *
 * @param ubend
 * @return unsigned long
 */
unsigned long ubend_size(struct Ubend const *ubend);

/**
 * @brief Insert a new value in the container
 *
 * @param ubend
 * @param x
 * @return the erased data or NaN
 */
double ubend_push(struct Ubend *ubend, double x);

#endif // UBEND_H
