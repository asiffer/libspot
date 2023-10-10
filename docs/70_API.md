---
title: API
---

## Typedefs

<div id="spot_8h_1af34cbecc36e51f2981fc6a4f2ccaaa12"></div>
??? typedef "malloc_fn"
    ```c
    typedef void *(* malloc_fn) (__SIZE_TYPE__)
    ```
    
    malloc_fn is a pointer to a malloc-type function i.e. with prototype: void * malloc(unsigned long)
    


<div id="spot_8h_1ac728a1a879612a5ebb351581594841eb"></div>
??? typedef "free_fn"
    ```c
    typedef void(* free_fn) (void *)
    ```
    
    free_fn is a pointer to a free-type function i.e. with prototype: void free(void*)
    


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

<div id="spot_8h_1afe0033fc0d2a9089e4ca79c61537af94"></div>
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


<div id="spot_8h_1abbfb21d5bb94f98e29b28b0f8b1d104e"></div>
??? fun "spot_free"
    ```c
    void spot_free(struct [Spot](#structSpot) * spot)
    ```
    
    Free the tail data.
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |


<div id="spot_8h_1afd6c1e52d8c9418be86d139d7fc395bc"></div>
??? fun "spot_fit"
    ```c
    int spot_fit(struct [Spot](#structSpot) * spot, double * data, unsigned long size)
    ```
    
    Compute the first excess and anomaly thresholds based on training data.
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |
    | `data` | Buffer of input data |
    | `size` | Size of the buffer |


<div id="spot_8h_1afe66cf061e1eeb64d26151e5b902f9cd"></div>
??? fun "spot_step"
    ```c
    enum SpotResult spot_step(struct [Spot](#structSpot) * spot, double x)
    ```
    
    fit-predict step
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |
    | `x` | new value |


<div id="spot_8h_1ac4d40e7cecabcb54ddef63c12698a214"></div>
??? fun "spot_quantile"
    ```c
    double spot_quantile(struct [Spot](#structSpot) const * spot, double q)
    ```
    
    Compute the value zq such that P(X>zq) = q.
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |
    | `q` | Low probability (it must be within the tail) |


<div id="spot_8h_1a5ab632a45e410fc333ea5553b26a315e"></div>
??? fun "spot_probability"
    ```c
    double spot_probability(struct [Spot](#structSpot) const * spot, double z)
    ```
    
    Compute the probability p such that P(X>z) = p.
    
    | Parameter | Description |
    |-----------|-------------|
    | `spot` | [Spot](#structSpot) instance |
    | `z` | High quantile (it must be within the tail) |


<div id="spot_8h_1a3f0314da2c0eca019cd9af6bdb465b12"></div>
??? fun "set_allocators"
    ```c
    void set_allocators(malloc_fn m, free_fn f)
    ```
    
    Set the allocators object (malloc and free)
    
    | Parameter | Description |
    |-----------|-------------|
    | `m` | pointer to a "malloc" function |
    | `f` | pointer to a "free" function |


<div id="spot_8h_1ade83e5b02c26bcd8aeca25ba39b3be56"></div>
??? fun "libspot_version"
    ```c
    void libspot_version(char * buffer, unsigned long size)
    ```
    
    Return the version of libspot.
    
    | Parameter | Description |
    |-----------|-------------|
    | `buffer` | input buffer to fill with |
    | `size` | size of the input buffer |


<div id="spot_8h_1a72ccd22bfc5bf72474897bd41be8f43f"></div>
??? fun "libspot_license"
    ```c
    void libspot_license(char * buffer, unsigned long size)
    ```
    
    Return the license of the library.
    
    | Parameter | Description |
    |-----------|-------------|
    | `buffer` | input buffer to fill with |
    | `size` | size of the input buffer |


<div id="spot_8h_1a33453c918db77a6b0058822d5e424d70"></div>
??? fun "error_msg"
    ```c
    void error_msg(enum LibspotError err, char * buffer, unsigned long size)
    ```
    
    Return a string related to an error code.
    
    | Parameter | Description |
    |-----------|-------------|
    | `err` | error code |
    | `buffer` | input buffer to fill with |
    | `size` | size of the input buffer |


