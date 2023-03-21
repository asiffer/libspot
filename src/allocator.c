/**
 * @file allocator.c
 * @brief Implements allocation utilities
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU General Public License version 3
 *
 */
#include "allocator.h"

/**
 * @brief Global allocator
 *
 */
static malloc_fn mfn;

/**
 * @brief Global free
 *
 */
static free_fn ffn;

void set_allocators(malloc_fn m, free_fn f) {
    mfn = m;
    ffn = f;
}

void *xmalloc(unsigned long size) {
    if (!mfn) {
        return 0x0;
    }
    return mfn(size);
}

void xfree(void *p) {
    if (!ffn) {
        return;
    }
    ffn(p);
}
