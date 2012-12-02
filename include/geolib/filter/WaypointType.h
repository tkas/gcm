#ifndef _GCM_GEOLIB_FILTER_WAYPOINTTYPE_H
# define _GCM_GEOLIB_FILTER_WAYPOINTTYPE_H

#include <geolib/Filter.h>
#include <stl/String.h>
#include <map>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API WaypointType: public Filter {
	public:
		static const int ID;
		virtual int getId() const;
		virtual String getName() const;
		virtual bool matchCache(GC<Geocache> cache);
		virtual bool matchWaypoint(GC<GeocacheWaypoint> waypoint);
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> waypoint);
		void include(GeocacheWaypoint::Type value);
		void exclude(GeocacheWaypoint::Type value);
		bool contains(GeocacheWaypoint::Type value);
		void clear();
		virtual void toXml(_xmlNode *root);
		virtual bool fromXml(_xmlNode *root);

	private:
		std::map<GeocacheWaypoint::Type, bool> list;
};

		}
	}
}

#endif
