/**
 * \file
 *
 * GCM Main header
 *
 * Main header file to include full GCM API.
 *
 * @author Michal Kuchta
 * @version 1.0
 */

#ifndef _GCM_H
#define _GCM_H

// Prefix for functions that are exported from modules.
#ifdef _WIN32
#	include <windows.h>
#	define PLUGIN_EXPORT extern "C" __declspec(dllexport)
#	ifdef LIBGCM
#		define GCM_API __declspec(dllexport)
#	else
#		define GCM_API __declspec(dllimport)
#	endif
#else
#	define PLUGIN_EXPORT extern "C"
#	define GCM_API
#endif

// Cast for passing callback functions
#define callback(func) (void *)(func)

#include "config.h"

/**
 * The application entry point in the libgcm.
 */
int GCM_API gcmcore_run(int argc, char *argv[]);

/**
 * Return GCM version.
 */
const char *gcm_version();

#include <time.h>

#ifdef _WIN32
# define snprintf _snprintf
#endif

/**
 * If localtime_r does not exists, supply own version.
 */
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
// Here, locatime_r should be defined.
#else
# ifndef _WIN32
struct tm GCM_API *localtime_r(const time_t *timep, struct tm *result);
struct tm GCM_API *gmtime_r(const time_t *timep, struct tm *result);
# endif
#endif

#ifdef _WIN32
#define true 1
#define false 0

char GCM_API *getcwd(char *buffer, size_t size);
int GCM_API chdir(const char *buffer);
#endif

#endif // gcm_h
