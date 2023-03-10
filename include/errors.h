/**
 * @file errors.h
 * @brief Declare libspot errors
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0a
 * @date Fri Mar 10 09:44:55 AM UTC 2023
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

const char *error_msg(enum LibspotError err);

#endif