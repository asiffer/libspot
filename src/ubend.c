#include "ubend.h"

int ubend_init(struct Ubend *ubend, unsigned long capacity) {
    ubend->cursor = 0;
    ubend->filled = 0;
    ubend->capacity = capacity;
    ubend->last_erased_data = _NAN;
    ubend->data = (double *)xmalloc(capacity * sizeof(double));
    if (ubend->data) {
        return 0;
    }
    return -ERR_ALLOC_ERROR;
}

void ubend_free(struct Ubend *ubend) {
    ubend->cursor = 0;
    ubend->capacity = 0;
    ubend->filled = -1;
    ubend->last_erased_data = _NAN;
    if (ubend->data) {
        xfree(ubend->data);
    }
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
