/**
 * @file allocator.h
 * @author your name (you@domain.com)
 * @brief
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "structs.h"

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

/**
 * @brief Internal function to set the allocators object (malloc and free)
 *
 * @param m pointer to a "malloc" function
 * @param f pointer to a "free" function
 */
void internal_set_allocators(malloc_fn m, free_fn f);

/**
 * @brief
 *
 * @param size
 * @return void*
 */
void *xmalloc(unsigned long size);

/**
 * @brief
 *
 * @param p
 */
void xfree(void *p);

#endif // ALLOCATOR_H
