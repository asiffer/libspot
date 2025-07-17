/**
 * @file allocator.c
 * @brief Implements allocation utilities
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b4
 * @date jeu. 17 juil. 2025 08:08:51 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "allocator.h"

/**
 * @brief Global allocator
 *
 */
static malloc_fn libspot_malloc;

/**
 * @brief Global free
 *
 */
static free_fn libspot_free;

void internal_set_allocators(malloc_fn m, free_fn f) {
    libspot_malloc = m;
    libspot_free = f;
}

void *xmalloc(unsigned long size) {
    if (!libspot_malloc) {
        return 0x0;
    }
    // here is a cast from 'unsigned long' to 'size_t'
    // In general it does not create issue because size_t
    // is bigger than 'unsigned long'. In some case this is
    // not the case (like arduino).
    // See https://stackoverflow.com/a/131833
    if (size > __SIZE_MAX__) {
        return 0x0;
    }
    return libspot_malloc(size);
}

void xfree(void *p) {
    if (!libspot_free) {
        return;
    }
    libspot_free(p);
}
