#ifndef _GCM_GEOLIB_FILTER_COMBINED_H
# define _GCM_GEOLIB_FILTER_COMBINED_H

#include <geolib/Filter.h>
#include <stl/List.h>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API Combined: public Filter, public List<Filter> {
	public:
		static const int ID;
		virtual int getId() const;
		virtual String getName() const;
		virtual bool matchCache(GC<Geocache> cache);
		virtual bool matchWaypoint(GC<GeocacheWaypoint> waypoint);
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> waypoint);
		virtual void toXml(_xmlNode *root);
		virtual bool fromXml(_xmlNode *root);
};

		}
	}
}

#endif
