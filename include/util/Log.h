#ifndef _GCM_UTIL_LOG_H
# define _GCM_UTIL_LOG_H 1

#include <gcm.h>
#include <stdio.h>
#include <stdarg.h>

#include <stl/String.h>
#include <Events.h>

namespace GCM {
	namespace util {

/**
 * Logging facility
 */
class GCM_API Log {
	private:
		static FILE *debugOut;
		static FILE *errorOut;

	public:
		struct EventParams {
			String section;
			String message;
		};
		static Events events;

		static void vd(const char *part, const char *fmt, va_list ap);
		static void ve(const char *part, const char *fmt, va_list ap);

		static void openDebugOut(char *out);
		static void openErrorOut(char *out);

		/**
		 * Debug log
		 */
		static void d(const char *part, const char *fmt, ...);

		/**
		 * Error log
		 */
		static void e(const char *part, const char *fmt, ...);
};

	}
}

#endif
