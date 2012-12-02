#ifndef _GCM_TEMPLATER_LIBRARY_H
# define _GCM_TEMPLATER_LIBRARY_H

#include "Variable.h"

namespace GCM {
	namespace templater {

#define FFUNC(name) GCM::GC<Variable> name(GCM::GC<Variable> var)
#define REGFFUNC(name) Templater::registerFormatFunc(#name, name)

static class Library {
	public:
		Library();
		
		// String functions
		static FFUNC(nl2br);
		static FFUNC(htmlspecialchars);
		static FFUNC(length);
} library;

	}
}

#endif
