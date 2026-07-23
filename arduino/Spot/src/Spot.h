
/**
 * @brief \`frexp_fn\` is a pointer to a frexp-type function
 * i.e. with prototype:
 * \`double frexp_fn(double, int*)\`
 *
 * This function decomposes a floating point value
 * into a normalized fraction and an integral power of 2.
 * See https://en.cppreference.com/w/c/numeric/math/frexp
 */
typedef double (*frexp_fn)(double, int *);

/**
 * @brief math_fn is a pointer to a generic real math function
 * like log, exp...
 */
typedef double (*math_fn)(double);

/**
 * @brief math_fn is a pointer to a generic two parameters
 * math function like pow
 */
typedef double (*math2_fn)(double, double);

/**
 * @brief \`ldexp_fn\` is a pointer to a ldexp-type function
 * i.e. with prototype:
 * \`double ldexp_fn(double, int*)\`
 *
 * This function multiplies a floating point value
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
 *
 * First it is empty. Then we feed it with data until its max capacity is
 * reached (transitory state). When it is full, the earlier data erase the
 * older one (cruise state).
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

// Spot API ------------------------------------------------------------------

/**
 * @brief Initialize the Spot structure
 *
 * @param spot Spot instance
 * @param q Decision probability (Spot will flag extreme events that will have
 * a probability lower than q)
 * @param low Lower tail mode (0 by defaut for upper tail and 1 for lower tail)
 * @param discard_anomalies Do not include anomalies in the model (default: 1,
 * 0 otherwise)
 * @param level Excess level (it is a high quantile that delimits the tail)
 * @param buffer Buffer to store all the excesses (tail data)
 * @param max_excess Size of the buffer (number of elements)
 * @retval 0 OK
 * @retval -ERR_LEVEL_OUT_OF_BOUNDS the level parameter is not between 0 and 1
 * @retval -ERR_Q_OUT_OF_BOUNDS the q parameter is not between 0 and 1-level
 * @retval -ERR_MEMORY_ALLOCATION_FAILED the tail data allocation failed
 */
int spot_init(struct Spot *spot, double q, int low, int discard_anomalies,
              double level, double *buffer, unsigned long max_excess);

/**
 * @brief Reset the internal structure of the Spot instance. It reuses the same
 * backing buffer.
 *
 * @param spot Spot instance
 */
void spot_reset(struct Spot *spot);

/**
 * @brief Compute the first excess and anomaly thresholds based on training
 * data
 *
 * @param spot Spot instance
 * @param data Buffer of input data
 * @param size Size of the buffer
 * @retval 0 OK
 * @retval -ERR_EXCESS_THRESHOLD_IS_NAN the excess threshold is nan
 * @retval -ERR_ANOMALY_THRESHOLD_IS_NAN the anomaly threshold is nan
 */
int spot_fit(struct Spot *spot, double const *data, unsigned long size);

/**
 * @brief fit-predict step
 *
 * @param spot Spot instance
 * @param x new value
 * @retval -ERR_DATA_IS_NAN input data is nan
 * @retval NORMAL data is normal
 * @retval EXCESS data lives in the tail
 * @retval ANOMALY data is out of the threshold
 */
int spot_step(struct Spot *spot, double x);

/**
 * @brief Compute the value zq such that P(X>zq) = q
 *
 * @param spot Spot instance
 * @param q Low probability (it must be within the tail)
 * @return the desired quantile
 */
double spot_quantile(struct Spot const *spot, double q);

/**
 * @brief Compute the probability p such that P(X>z) = p
 *
 * @param spot Spot instance
 * @param z High quantile (it must be within the tail)
 * @return the desired probability
 */
double spot_probability(struct Spot const *spot, double z);

/* Extra functions */

/**
 * @brief Set the ldexp/frexp functions
 *
 * By default these functions are provided but the API
 * allows to change them. It is low level, you might rather
 * use set_math_functions to provide optimized math functions.
 *
 * @param l pointer to a `ldexp` function (`NULL` if you do not want to change
 * it)
 * @param f pointer to a `frexp` function (`NULL` if you do not want to change
 * it)
 */
void set_float_utils(ldexp_fn l, frexp_fn f);

/**
 * @brief Set the log, exp and pow functions. It is recommended when you have
 * access to optimized versions (e.g., from the standard library).
 *
 * @param lo pointer to a `log` function (`NULL` if you do not want to change
 * it)
 * @param ex pointer to a `exp` function (`NULL` if you do not want to change
 * it)
 * @param po pointer to a `pow` function (`NULL` if you do not want to change
 * it)
 */
void set_math_functions(math_fn lo, math_fn ex, math2_fn po);

/**
 * @brief Return the version of libspot
 *
 * @param[out] buffer input buffer to fill with
 * @param size size of the input buffer
 */
void libspot_version(char *buffer, unsigned long size);

/**
 * @brief Return the license of the library
 *
 * @param[out] buffer input buffer to fill with
 * @param size size of the input buffer
 */
void libspot_license(char *buffer, unsigned long size);

/**
 * @brief Return a string related to an error code
 *
 * @param err error code
 * @param[out] buffer input buffer to fill with
 * @param size size of the input buffer
 */
void libspot_error(enum LibspotError err, char *buffer, unsigned long size);
