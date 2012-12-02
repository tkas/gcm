#ifndef _GCM_GEOLIB_FILTER_SIZE_H
# define _GCM_GEOLIB_FILTER_SIZE_H

#include <geolib/Filter.h>
#include <stl/String.h>
#include <map>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API Size: public Filter {
	public:
		static const int ID;
		virtual int getId() const;
		virtual String getName() const;
		virtual bool matchCache(GC<Geocache> cache);
		virtual bool matchWaypoint(GC<GeocacheWaypoint> waypoint);
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> waypoint);
		void include(Geocache::Size value);
		void exclude(Geocache::Size value);
		bool contains(Geocache::Size value);
		void clear();
		virtual void toXml(_xmlNode *root);
		virtual bool fromXml(_xmlNode *root);

	private:
		std::map<Geocache::Size, bool> list;
};

		}
	}
}

#endif
