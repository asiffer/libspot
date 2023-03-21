/**
 * @file errors.h
 * @brief Declare libspot errors
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU General Public License version 3
 *
 */

#ifndef ERRORS_H
#define ERRORS_H

enum LibspotError {
    ERR_MEMORY_ALLOCATION_FAILED = 1000,
    ERR_LEVEL_OUT_OF_BOUNDS,
    ERR_Q_OUT_OF_BOUNDS,
    ERR_EXCESS_THRESHOLD_IS_NAN,
    ERR_ANOMALY_THRESHOLD_IS_NAN,
    ERR_DATA_IS_NAN,
};

/**
 * @brief Return a string related to an error code
 *
 * @param err error code
 * @return error message
 */
const char *error_msg(enum LibspotError err);

#endif