/**
 * @file allocator.h
 * @author your name (you@domain.com)
 * @brief
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

/**
 * @brief malloc_fn is a pointer to a malloc-type function
 * i.e. with prototype:
 * void * malloc(unsigned long)
 * @details __SIZE_TYPE__ define the size_t type. This type is notably used
 * for malloc but it depends on the architecture. As we assume we receive
 * malloc-like functions we have to be compliant with their prototypes.
 */
typedef void *(*malloc_fn)(__SIZE_TYPE__);

/**
 * @brief free_fn is a pointer to a free-type function
 * i.e. with prototype:
 * void free(void*)
 */
typedef void (*free_fn)(void *);

/**
 * @brief Set the allocators object (malloc and free)
 * wtf
 *
 * @param m pointer to a "malloc" function
 * @param f pointer to a "free" function
 */
void set_allocators(malloc_fn m, free_fn f);

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
