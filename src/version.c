/**
 * @file version.c
 * @brief Implements license and version callbacks
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU General Public License version 3
 *
 */
#include "version.h"

static const char *version = VERSION;
static const char *license = LICENSE;

static unsigned long const version_size = sizeof(VERSION) / sizeof(char);
static unsigned long const license_size = sizeof(LICENSE) / sizeof(char);

void libspot_version(char *buffer) {
    for (unsigned long i = 0; i < version_size; i++) {
        buffer[i] = version[i];
    }
}

void libspot_license(char *buffer) {
    for (unsigned long i = 0; i < license_size; i++) {
        buffer[i] = license[i];
    }
}

/**
- call from python -

from ctypes import CDLL, create_string_buffer, c_char_p
libspot = CDLL("./lib/libspot.so")

libspot.libspot_version.argtypes = [c_char_p]
libspot.libspot_version.restype = None

buffer = create_string_buffer(32)
libspot.libspot_version(buffer)
print(buffer.value.decode())
**/
