#include <util/Log.h>
#include <stdio.h>
#include <stdarg.h>
#include <gcm.h>

#ifndef _WIN32
# include <sys/time.h>
#else
# include "gettimeofday.h"
#endif

using namespace GCM;
using namespace GCM::util;

Events Log::events;

FILE GCM_API *Log::debugOut = NULL;
FILE GCM_API *Log::errorOut = NULL;

struct timeval startup;

/* openDebugOut() - initialize debug output
 *
 * out == NULL   - do not produce debug output
 * out == ""     - print debug output to stdout
 * out == "FILE" - write debug output to FILE
 */
void GCM_API Log::openDebugOut(char *out) {
	if (debugOut != NULL && debugOut != stdout && debugOut != stderr) {
		fclose(debugOut);
	}

	gettimeofday(&startup, NULL);

  // disable debug output
  if(out == NULL) 
  	debugOut = NULL;
  // debug output to stdout - empty string
  else if(out[0] == '\0') 
  	debugOut = stdout;
  // debug output to some file
  else {
    debugOut = fopen(out, "a");
  	if (debugOut == NULL){
      Log::e("Unable to open file %s for debug output!\n", out);
      exit(1); //FIXME - proper exit code;
    }
  }
}

/* openErrorOut() - initialize error output
 *
 * out == NULL   - do not produce error output
 * out == ""     - print error output to stderr
 * out == "FILE" - write error output to FILE
 */
void GCM_API Log::openErrorOut(char *out) {
	if (errorOut != NULL && errorOut != stdout && errorOut != stderr) {
		fclose(errorOut);
	}

  // disable debug output
  if(out == NULL) 
  	errorOut = NULL;
  // debug output to stdout - empty string
  else if(out[0] == '\0') 
	  errorOut = stderr;
  // debug output to some file
  else {
    errorOut = fopen(out, "a");
  	if (errorOut == NULL){
      fprintf(stderr, "Unable to open file %s for error output!\n", out);
      exit(1); //FIXME - proper exit code
    }
  }
}

void GCM_API Log::vd(const char *part, const char *fmt, va_list ap) {
	if (debugOut == NULL) return;

	struct timeval tm;
	gettimeofday(&tm, NULL);

	fprintf(debugOut, "[%ld] [%s] DEBUG: ", (tm.tv_sec - startup.tv_sec) * 1000 + (tm.tv_usec - startup.tv_usec) / 1000, part);
	vfprintf(debugOut, fmt, ap);
	fprintf(debugOut, "\n");
}

void GCM_API Log::d(const char *part, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vd(part, fmt, ap);
	va_end(ap);
}

void GCM_API Log::ve(const char *part, const char *fmt, va_list ap) {
	if (errorOut == NULL) return;

	Log::EventParams params;
	params.section = part;
	params.message = String::vformat(fmt, ap);
	Log::events.trigger("error", &params);

	fprintf(errorOut, "[%s] ERROR: %s\n", part, params.message->c_str());
}

void GCM_API Log::e(const char *part, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	ve(part, fmt, ap);
	va_end(ap);
}
