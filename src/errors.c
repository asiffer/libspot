/**
 * @file errors.c
 * @brief Implements error message retrieval
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU General Public License version 3
 *
 */
#include "errors.h"

const char *error_msg(enum LibspotError err) {
    switch (err) {
    case ERR_MEMORY_ALLOCATION_FAILED:
        return "Memory allocation failed";
    case ERR_LEVEL_OUT_OF_BOUNDS:
        return "The level parameter is out of bounds (it must be between 0 "
               "and 1, but close to 1)";
    case ERR_Q_OUT_OF_BOUNDS:
        return "The q parameter must between 0 and 1-level";
    case ERR_EXCESS_THRESHOLD_IS_NAN:
        return "The excess threshold has not been initialized";
    case ERR_ANOMALY_THRESHOLD_IS_NAN:
        return "The anomaly threshold has not been initialized";
    case ERR_DATA_IS_NAN:
        return "The input data is NaN";
    default:
        return 0;
    }
}
