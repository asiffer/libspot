/**
 * @file allocator.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-09-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

//

/**
 * @brief malloc_fn is a pointer to a malloc-type function
 * i.e. with prototype:
 * void * malloc(unsigned long)
 *
 */
typedef void *(*malloc_fn)(unsigned long);

/**
 * @brief free_fn is a pointer to a free-type function
 * i.e. with prototype:
 * void free(void*)
 */
typedef void (*free_fn)(void *);

/**
 * @brief Set the allocators object (malloc and free)
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
