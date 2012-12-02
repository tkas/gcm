#ifndef _GCM_GEOLIB_FILTER_GEOCACHETYPE_H
# define _GCM_GEOLIB_FILTER_GEOCACHETYPE_H

#include <geolib/Filter.h>
#include <stl/String.h>
#include <map>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API GeocacheType: public Filter {
	public:
		static const int ID;
		virtual int getId() const;
		virtual String getName() const;
		virtual bool matchCache(GC<Geocache> cache);
		virtual bool matchWaypoint(GC<GeocacheWaypoint> waypoint);
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> waypoint);
		void include(Geocache::Type value);
		void exclude(Geocache::Type value);
		bool contains(Geocache::Type value);
		void clear();
		virtual void toXml(_xmlNode *root);
		virtual bool fromXml(_xmlNode *root);

	private:
		std::map<Geocache::Type, bool> list;
};

		}
	}
}

#endif
