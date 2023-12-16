/**
 * @file structs.h
 * @brief Declare all the base objects
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */

#ifndef STRUCTS_H
#define STRUCTS_H

/**
 * @brief \`malloc_fn\` is a pointer to a malloc-type function
 * i.e. with prototype:
 * \`void * malloc(size_t)\`
 * @details __SIZE_TYPE__ define the size_t type. This type is notably used
 * for malloc but it depends on the architecture. As we assume we receive
 * malloc-like functions we have to be compliant with their prototypes.
 */
typedef void *(*malloc_fn)(__SIZE_TYPE__);

/**
 * @brief \`free_fn\` is a pointer to a free-type function
 * i.e. with prototype:
 * \`void free(void*)\`
 */
typedef void (*free_fn)(void *);

/**
 * @brief \`frexp_fn\` is a pointer to a frexp-type function
 * i.e. with prototype:
 * \`double frexp_fn(double, int*)\`
 * @details This function decomposes a floating point value
 * into a normalized fraction and an integral power of 2.
 * See https://en.cppreference.com/w/c/numeric/math/frexp
 */
typedef double (*frexp_fn)(double, int *);

/**
 * @brief \`ldexp_fn\` is a pointer to a ldexp-type function
 * i.e. with prototype:
 * \`double ldexp_fn(double, int*)\`
 * @details This function multiplies a floating point value
 * by the number 2 raised to the exp power.
 * See https://en.cppreference.com/w/c/numeric/math/ldexp
 */
typedef double (*ldexp_fn)(double, int);

/**
 * @brief \`real_function\` defines a pointer to a univariate function that
 * can require extra paramaters (second argument)
 */
typedef double (*real_function)(double, void *);

/**
 * @brief Constants to store libspot errors
 *
 */
enum LibspotError {
    /// Allocation of the backing array failed
    ERR_MEMORY_ALLOCATION_FAILED = 1000,
    /// The level parameter must between 0 and 1
    ERR_LEVEL_OUT_OF_BOUNDS,
    /// The q parameter must be between 0 and 1-level
    ERR_Q_OUT_OF_BOUNDS,
    /// The excess threshold has not been initialized
    ERR_EXCESS_THRESHOLD_IS_NAN,
    /// The anomaly threshold has not been initialized
    ERR_ANOMALY_THRESHOLD_IS_NAN,
    /// The input data is NaN
    ERR_DATA_IS_NAN,
};

/**
 * @brief Possible outputs of a Spot step
 *
 */
enum SpotResult {
    /// @brief Data is normal
    NORMAL = 0,
    /// @brief Data is in the tail (so the model has been updated)
    EXCESS = 1,
    /// @brief Data is beyond the anomaly threshold
    ANOMALY = 2,
};

/**
 *  @brief This container is a kind of circular vector.
 *  @details First it is empty.
 *  Then we feed it with data until its max capacity is reached (transitory
 * state). When it is full, the earlier data erase the older one (cruise
 * state).
 */
struct Ubend {
    /// @brief Current position inside the container
    unsigned long cursor;
    /// @brief Max storage
    unsigned long capacity;
    /// @brief Last erased value (i.e. replaced by a new one)
    double last_erased_data;
    /// @brief Container fill status (1 = filled, 0 = not filled)
    int filled;
    /// @brief Data container
    double *data;
};

/**
 * @brief Stucture that computes stats about the peaks
 *
 */
struct Peaks {
    /// @brief Sum of the elements
    double e;
    /// @brief Sum of the square of the elements
    double e2;
    /// @brief Minimum of the elements
    double min;
    /// @brief Maximum of the elements
    double max;
    /// @brief Underlying data container
    struct Ubend container;
};

/**
 * @brief Stucture that embeds GPD parameter (GPD tail actually)
 *
 */
struct Tail {
    /// @brief GPD gamma parameter
    double gamma;
    /// @brief GPD sigma parameter
    double sigma;
    /// @brief Underlyning Peaks structure
    struct Peaks peaks;
};

/**
 * @struct Spot
 * @brief Main structure to run the SPOT algorithm
 *
 */
struct Spot {
    /// @brief Probability of an anomaly
    double q;
    /// @brief Location of the tail (high quantile)
    double level;
    /// @brief Flag anomalies (1 = flag, 0 = don't flag)
    int discard_anomalies;
    /// @brief Upper/Lower tail choice (1 = lower tail, 0 = upper tail)
    int low;
    /// @brief Internal constant (+/- 1.0)
    double __up_down;
    /// @brief  Normal/abnormal threshold
    double anomaly_threshold;
    /// @brief  Tail threshold
    double excess_threshold;
    /// @brief Total number of excesses
    unsigned long Nt;
    /// @brief Total number of seen data
    unsigned long n;
    /// @brief GPD Tail
    struct Tail tail;
};

#endif // STRUCTS_H
