#include "../dist/spot.h"
#include "stdio.h"
#include "stdlib.h"
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
struct Spot *spot_new(void) { return malloc(sizeof(struct Spot)); }

EMSCRIPTEN_KEEPALIVE
__SIZE_TYPE__ spot_size(void) { return sizeof(struct Spot); }

// see
// https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html
int main(void) {
    // provide allocators
    set_allocators(malloc, free);
}
