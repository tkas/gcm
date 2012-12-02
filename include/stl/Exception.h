#ifndef _GCM_EXCEPTION_H
# define _GCM_EXCEPTION_H

#include <gcm.h>
#include <stdio.h>
#include <stdlib.h>
#include <exception>
#ifndef _WIN32
# include <execinfo.h>
#endif

namespace GCM {

class GCM_API Exception: public std::exception {
	protected:
		int nSize;
		char **symbols;
		void *array[25];

	public:
		Exception(): std::exception() {
#ifndef _WIN32
			void *array[25];
			this->nSize = backtrace(array, 25);
			this->symbols = backtrace_symbols(array, this->nSize);
#endif
		}

		void printBacktrace() {
			this->printBacktrace(stderr);
		}

		void printBacktrace(FILE *out) {
#ifndef _WIN32
			fprintf(out, "Debug backtrace (most recent call first)\n");
			for (int i = 0; i < this->nSize; i++) {
				fprintf(out, "%d. %s\n", i + 1, this->symbols[i]);
			}
#else
			(void)out;
#endif
		}

		~Exception() throw() {
			free(symbols);
		}
};

}

#endif
