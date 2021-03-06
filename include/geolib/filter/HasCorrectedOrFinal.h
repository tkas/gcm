#ifndef _GCM_GEOLIB_FILTER_HASCORRECTEDORFINAL_H
# define _GCM_GEOLIB_FILTER_HASCORRECTEDORFINAL_H

#include <geolib/Filter.h>
#include <stl/String.h>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API HasCorrectedOrFinal: public Filter {
	public:
		static const int ID;
		virtual int getId() const;
		virtual String getName() const;
		virtual bool matchCache(GC<Geocache> cache);
		virtual bool matchWaypoint(GC<GeocacheWaypoint> waypoint);
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> waypoint);
};

		}
	}
}

#endif
