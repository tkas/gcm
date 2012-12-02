/*
 * NoPluginSelectedException.h
 *
 *  Created on: Feb 21, 2012
 *      Author: Niximor
 */

#ifndef NOPLUGINSELECTEDEXCEPTION_H_
#define NOPLUGINSELECTEDEXCEPTION_H_

#include <gcm.h>

namespace GCM {
	namespace pluginmanager {

class GCM_API NoPluginSelectedException: public std::exception {
	virtual const char *what() const throw() {
		return "No plugin was selected.";
	}
};

	}
}

#endif /* NOPLUGINSELECTEDEXCEPTION_H_ */
