/**
 * @file ubend.h
 * @brief Declares Ubend structure and methods
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU Lesser General Public License v3.0
 */

#include "allocator.h"
#include "errors.h"
#include "xmath.h"

#ifndef UBEND_H
#define UBEND_H

/**
 *  @brief This container is a kind of circular vector.
 *  @details First it is empty.
 *  Then we feed it with data until its max capacity is reached (transitory
 * state). When it is full, the earlier data erase the older one (cruise
 * state).
 */
struct Ubend {
    unsigned long cursor;    /* current position inside the container */
    double *data;            /* data storage */
    unsigned long capacity;  /* max storage */
    double last_erased_data; /* last erased value (replaced by a new one) */
    int filled;              /* container fill status */
};

/**
 * @brief Ubend structure initializer
 *
 * @param ubend structure to init
 * @param capacity number of double to allocate
 * @return 0 is allocation is successful
 */
int ubend_init(struct Ubend *ubend, unsigned long capacity);

/**
 * @param ubend
 */
void ubend_free(struct Ubend *ubend);

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
