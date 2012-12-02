#ifndef _GCM_GEOLIB_LOGLIST_H
# define _GCM_GEOLIB_LOGLIST_H

#include <gcm.h>
#include <stl/List.h>
#include "Log.h"

namespace GCM {
	namespace geolib {

class LogList: public List<Log> {
	public:
		virtual GC<Log> append(GC<Log> log);
};

	}
}

#endif
