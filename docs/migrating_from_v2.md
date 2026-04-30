---
title: Migrating from v2
summary: No worries
---

The brand new `v3` version removes the allocator API.
If you use the [typescript](./foreign/javascript.md) or [python](./foreign/python.md) versions, **you have nothing to do**!

**Before**

With thr allocator API, the user passed `malloc` and `free` functions so that `libspot` allocates the buffer to store tail data based on `max_excess`.

```c
set_allocators(malloc, free); 

struct Spot spot;

int status = spot_init(
        &spot,  // pointer to the allocated structure
        1e-4,   // q: anomaly probability
        0,      // low: observe upper tail
        1,      // discard_anomalies: flag anomalies
        0.998,  // level: tail quantile (the 0.2% higher values shapes the tail)
        200     // max_excess: number of data to keep to summarize the tail
    );
```

**After**

Henceforth, the user directly provides the buffer.

```c
struct Spot spot;

double buffer[200]; // <-- allocate tail buffer on your own

int status = spot_init(
        &spot,  
        1e-4,   
        0,      
        1,      
        0.998,  
        buffer, // buffer: backing array to store the excesses (now provided by the user)
        200     
    );
```

!!! warning "Important:"
    The `max_excess` parameter is the size of the `buffer`.