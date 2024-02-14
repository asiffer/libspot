---
title: API
---

## Typedefs

<div id="spot_8h_1af34cbecc36e51f2981fc6a4f2ccaaa12"></div>
??? typedef "malloc_fn"
    ```c
    typedef void *(* malloc_fn) (__SIZE_TYPE__)
    ```
    
    `malloc_fn` is a pointer to a malloc-type function i.e. with prototype: `void * malloc(size_t)`
    


<div id="spot_8h_1ac728a1a879612a5ebb351581594841eb"></div>
??? typedef "free_fn"
    ```c
    typedef void(* free_fn) (void *)
    ```
    
    `free_fn` is a pointer to a free-type function i.e. with prototype: `void free(void*)`
    


<div id="spot_8h_1ad948bdfab02151949931ddd4cbb4c9d8"></div>
??? typedef "frexp_fn"
    ```c
    typedef double(* frexp_fn) (double, int *)
    ```
    
    `frexp_fn` is a pointer to a frexp-type function i.e. with prototype: `double frexp_fn(double, int*)`
    


<div id="spot_8h_1afe19bf670d7dd3934150c80e165c3441"></div>
??? typedef "ldexp_fn"
    ```c
    typedef double(* ldexp_fn) (double, int)
    ```
    
    `ldexp_fn` is a pointer to a ldexp-type function i.e. with prototype: `double ldexp_fn(double, int*)`
    


<div id="spot_8h_1a02bc38a8011adaf2313cb095d70eaf43"></div>
??? typedef "real_function"
    ```c
    typedef double(* real_function) (double, void *)
    ```
    
    `real_function` defines a pointer to a univariate function that can require extra paramaters (second argument)
    


## Enum

<div id="spot_8h_1a3e77eff56146e2d86fc7d02819138a64"></div>
??? enum "LibspotError"
    ```c
    enum LibspotError {
        ERR_MEMORY_ALLOCATION_FAILED = 1000,
        ERR_LEVEL_OUT_OF_BOUNDS,
        ERR_Q_OUT_OF_BOUNDS,
        ERR_EXCESS_THRESHOLD_IS_NAN,
        ERR_ANOMALY_THRESHOLD_IS_NAN,
        ERR_DATA_IS_NAN,
    };
    ```
    
    Constants to store libspot errors.
    
    | Name | Description |
    |------|-------------|
    | `ERR_MEMORY_ALLOCATION_FAILED` | Allocation of the backing array failed. |
    | `ERR_LEVEL_OUT_OF_BOUNDS` | The level parameter must between 0 and 1. |
    | `ERR_Q_OUT_OF_BOUNDS` | The q parameter must be between 0 and 1-level. |
    | `ERR_EXCESS_THRESHOLD_IS_NAN` | The excess threshold has not been initialized. |
    | `ERR_ANOMALY_THRESHOLD_IS_NAN` | The anomaly threshold has not been initialized. |
    | `ERR_DATA_IS_NAN` | The input data is NaN. |


<div id="spot_8h_1a411400ee8f5004dad5990ed46ccf0596"></div>
??? enum "SpotResult"
    ```c
    enum SpotResult {
        NORMAL = 0,
        EXCESS = 1,
        ANOMALY = 2,
    };
    ```
    
    Possible outputs of a [Spot](#structSpot) step.
    
    | Name | Description |
    |------|-------------|
    | `NORMAL` | Data is normal. |
    | `EXCESS` | Data is in the tail (so the model has been updated) |
    | `ANOMALY` | Data is beyond the anomaly threshold. |


## Structures

<div id="structUbend"></div>
??? struct "Ubend"
    ```c
    struct Ubend {
        unsigned long cursor;
        unsigned long capacity;
        double last_erased_data;
        int filled;
        double * data;
    };
    ```
    
    This container is a kind of circular vector.
    
    First it is empty. Then we feed it with data until its max capacity is reached (transitory state). When it is full, the earlier data erase the older one (cruise state). 
    
    | Member | Description |
    |--------|-------------|
    | `cursor` | Current position inside the container. |
    | `capacity` | Max storage. |
    | `last_erased_data` | Last erased value (i.e. replaced by a new one) |
    | `filled` | Container fill status (1 = filled, 0 = not filled) |
    | `data` | Data container. |


<div id="structPeaks"></div>
??? struct "Peaks"
    ```c
    struct Peaks {
        double e;
        double e2;
        double min;
        double max;
        struct Ubend container;
    };
    ```
    
    Stucture that computes stats about the peaks.
    
    | Member | Description |
    |--------|-------------|
    | `e` | Sum of the elements. |
    | `e2` | Sum of the square of the elements. |
    | `min` | Minimum of the elements. |
    | `max` | Maximum of the elements. |
    | `container` | Underlying data container. |


<div id="structTail"></div>
??? struct "Tail"
    ```c
    struct Tail {
        double gamma;
        double sigma;
        struct Peaks peaks;
    };
    ```
    
    Stucture that embeds GPD parameter (GPD tail actually)
    
    | Member | Description |
    |--------|-------------|
    | `gamma` | GPD gamma parameter. |
    | `sigma` | GPD sigma parameter. |
    | `peaks` | Underlyning [Peaks](#structPeaks) structure. |


<div id="structSpot"></div>
??? struct "Spot"
    ```c
    struct Spot {
        double q;
        double level;
        int discard_anomalies;
        int low;
        double __up_down;
        double anomaly_threshold;
        double excess_threshold;
        unsigned long Nt;
        unsigned long n;
        struct Tail tail;
    };
    ```
    
    Main structure to run the SPOT algorithm.
    
    | Member | Description |
    |--------|-------------|
    | `q` | Probability of an anomaly. |
    | `level` | Location of the tail (high quantile) |
    | `discard_anomalies` | Flag anomalies (1 = flag, 0 = don't flag) |
    | `low` | Upper/Lower tail choice (1 = lower tail, 0 = upper tail) |
    | `__up_down` | Internal constant (+/- 1.0) |
    | `anomaly_threshold` | Normal/abnormal threshold. |
    | `excess_threshold` | [Tail](#structTail) threshold. |
    | `Nt` | Total number of excesses. |
    | `n` | Total number of seen data. |
    | `tail` | GPD [Tail](#structTail). |


## Functions

<div id="spot_init"></div>
??? fun "spot_init"
    ```c
    int spot_init(struct [Spot](#structSpot) * spot, double q, int low, int discard_anomalies, double level, unsigned long max_excess)
    ```
    
    Initialize the [Spot](#structSpot) structure.
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |
    | `q` | Decision probability ([Spot](#structSpot) will flag extreme events that will have a probability lower than q) |
    | `low` | Lower tail mode (0 by defaut for upper tail and 1 for lower tail) |
    | `discard_anomalies` | Do not include anomalies in the model (default: 1, 0 otherwise) |
    | `level` | Excess level (it is a high quantile that delimits the tail) |
    | `max_excess` | Maximum number of data that are kept to analyze the tail |


<div id="spot_free"></div>
??? fun "spot_free"
    ```c
    void spot_free(struct [Spot](#structSpot) * spot)
    ```
    
    Free the tail data.
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |


<div id="spot_fit"></div>
??? fun "spot_fit"
    ```c
    int spot_fit(struct [Spot](#structSpot) * spot, double const * data, unsigned long size)
    ```
    
    Compute the first excess and anomaly thresholds based on training data.
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |
    | `data` | Buffer of input data |
    | `size` | Size of the buffer |


<div id="spot_step"></div>
??? fun "spot_step"
    ```c
    int spot_step(struct [Spot](#structSpot) * spot, double x)
    ```
    
    fit-predict step
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |
    | `x` | new value |


<div id="spot_quantile"></div>
??? fun "spot_quantile"
    ```c
    double spot_quantile(struct [Spot](#structSpot) const * spot, double q)
    ```
    
    Compute the value zq such that P(X>zq) = q.
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |
    | `q` | Low probability (it must be within the tail) |


<div id="spot_probability"></div>
??? fun "spot_probability"
    ```c
    double spot_probability(struct [Spot](#structSpot) const * spot, double z)
    ```
    
    Compute the probability p such that P(X>z) = p.
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |
    | `z` | High quantile (it must be within the tail) |


<div id="set_allocators"></div>
??? fun "set_allocators"
    ```c
    void set_allocators(malloc_fn m, free_fn f)
    ```
    
    Set the allocators object (malloc and free)
    
    | Parameter | Description |
    |-----------|-------------|
    | `m` | pointer to a "malloc" function |
    | `f` | pointer to a "free" function |


<div id="set_float_utils"></div>
??? fun "set_float_utils"
    ```c
    void set_float_utils(ldexp_fn l, frexp_fn f)
    ```
    
    Set the ldexp/frexp functions.
    
    By default these functions are provided but the API allows to change them.
    
    | Parameter | Description |
    |-----------|-------------|
    | `l` | pointer to a "ldexp" function |
    | `f` | pointer to a "frexp" function |


<div id="libspot_version"></div>
??? fun "libspot_version"
    ```c
    void libspot_version(char * buffer, unsigned long size)
    ```
    
    Return the version of libspot.
    
    | Parameter | Description |
    |-----------|-------------|
    | `buffer` | input buffer to fill with |
    | `size` | size of the input buffer |


<div id="libspot_license"></div>
??? fun "libspot_license"
    ```c
    void libspot_license(char * buffer, unsigned long size)
    ```
    
    Return the license of the library.
    
    | Parameter | Description |
    |-----------|-------------|
    | `buffer` | input buffer to fill with |
    | `size` | size of the input buffer |


<div id="libspot_error"></div>
??? fun "libspot_error"
    ```c
    void libspot_error(enum LibspotError err, char * buffer, unsigned long size)
    ```
    
    Return a string related to an error code.
    
    | Parameter | Description |
    |-----------|-------------|
    | `err` | error code |
    | `buffer` | input buffer to fill with |
    | `size` | size of the input buffer |


