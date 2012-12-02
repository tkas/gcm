/*
 * UnsupportedFunctionException.h
 *
 *  Created on: Feb 21, 2012
 *      Author: Niximor
 */

#ifndef UNSUPPORTEDFUNCTIONEXCEPTION_H_
#define UNSUPPORTEDFUNCTIONEXCEPTION_H_

#include <gcm.h>

namespace GCM {
	namespace pluginmanager {

/**
 * Exception thrown when function is not implemented by any plugin.
 */
class GCM_API UnsupportedFunctionException: public std::exception {
	virtual const char *what() const throw() {
		return "There is no plugin for required function.";
	}
};

	}
}

#endif /* UNSUPPORTEDFUNCTIONEXCEPTION_H_ */
