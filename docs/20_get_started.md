---
title: Get started
---

Inside your code, you just have to add the `spot.h` header file.

```c
#include "spot.h"
```

One paramount point is that `libspot` does not know how to allocate/free memory (it does not know `libc` by design). So you have to provide these functions. By default you can pass the common `malloc` and `free` functions from `stdlib.h`.

```c
#include "spot.h"
#include <stdlib.h>

set_allocators(malloc, free);
```

Ok, now you want to use the SPOT algorithm. You can allocate a `Spot` object either on stack or on heap.

```c
// stack allocation
struct Spot spot;
// or heap allocation
struct Spot* spot_ptr = (struct Spot*)malloc(sizeof(struct Spot));
```

Then you must init that structure with the `spot_init` function.

```c
// here we assume stack allocation
struct Spot spot;
// init with SPOT parameters
int status = spot_init(
        &spot, // pointer to the allocated structure
        1e-4,  // q: anomaly probability
        0,     // low: observe upper tail
        1,     // discard_anomalies: flag anomalies
        0.998, // level: tail quantile (the 0.2% higher values shapes the tail)
        200    // max_excess: number of data to keep to summarize the tail
    );
// you can check the initialization
if (status < 0) {
    // print error
    char buffer[100];
    error_msg(-status, buffer, 100);
    printf("ERROR %d: %s\n", -status, buffer);
}
```

Basically `q` is the anomaly probability. The algorithm will flag events that have a lower probability than `q`. In practice, it must be very low (like `1e-3` or less).

The `low` parameter just defines whether we flag high (`low = 0`) of low (`low = 1`) values while `discard_anomalies` says that we want to reject anomalies.

The `level` should be a high quantile (a value close to `1`). It is useful to delimitate the tail of the distribution. One may use values like `0.98`, `0.99` or `0.995`.

Finally `max_excess` is the number of data that will be kept to model the tail of the distribution.

You can read more about the parameters in the [dedicated section](30_parameters.md).

Before prediction, we commonly need to fit the algorithm with first data. In practice you must provide a buffer of `double` (pointer + size of the buffer). How many records are needed? Briefly, few thousands (like 2000) but it depends on the parameters passed to SPOT (and also whether you have enough data).

```c
// double* initial_data = ...
// unsigned long size = ...
status = spot_fit(&spot, initial_data, size);
if (status < 0) {
    // print error
    char buffer[100];
    error_msg(-status, buffer, 100);
    printf("ERROR %d: %s\n", -status, buffer);
}
```

## Full example

Here we present a basic example where the SPOT algorithm is run on an exponential stream.

```c
--8<-- "examples/basic.c"
```
