/**
 * @file ubend.c
 * @brief Implements Ubend methods
 * @author Alban Siffer (31479857+asiffer@users.noreply.github.com)
 * @version 3.0.0
 * @date mar. 14 avril 2026 14:59:11 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "ubend.h"

int ubend_init(struct Ubend *ubend, double *buffer, unsigned long capacity) {
    ubend->cursor = 0;
    ubend->filled = 0;
    ubend->capacity = capacity;
    ubend->last_erased_data = _NAN;
    ubend->data = buffer;
    return 0;
}

unsigned long ubend_size(struct Ubend const *ubend) {
    if (ubend->filled) {
        return ubend->capacity;
    }
    return ubend->cursor;
}

double ubend_push(struct Ubend *ubend, double x) {
    // if the container has already been filled
    // we must keep in memory the data we will
    // erase
    if (ubend->filled) {
        ubend->last_erased_data = ubend->data[ubend->cursor];
    }

    // assign to value at cursor
    ubend->data[ubend->cursor] = x;

    // increment cursor
    if (ubend->cursor == (ubend->capacity - 1)) {
        ubend->cursor = 0;
        ubend->filled = 1;
    } else {
        ubend->cursor += 1;
    }

    return ubend->last_erased_data;
}
