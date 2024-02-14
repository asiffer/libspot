#include "../dist/spot.h"
#include "stdio.h"
#include "stdlib.h"
#include <emscripten.h>

struct Spot *spot_new(void) { return malloc(sizeof(struct Spot)); }

__SIZE_TYPE__ spot_size(void) { return sizeof(struct Spot); }

void libspot_init(void) {
    // provide allocators
    set_allocators(malloc, free);
}

// see
// https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html
int main(void) {
    libspot_init();
    return 0;
}
