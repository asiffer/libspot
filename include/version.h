/**
 * @file version.h
 * @brief Declares version and license
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */

#ifndef VERSION
#define VERSION "2.0a"
#endif

#ifndef LICENSE
#define LICENSE "GPL-3.0-or-later"
#endif

#ifndef VERSION_H
#define VERSION_H

/**
 * @brief Return the version of libspot
 *
 * @param[out] buffer input buffer to fill with
 */
void libspot_version(char *buffer);

/**
 * @brief Return the license of the library
 *
 * @param[out] buffer input buffer to fill with
 */
void libspot_license(char *buffer);

#endif // UBEND_H
