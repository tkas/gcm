#ifndef _GCM_GEOLIB_FILTER_PLACEDBY_H
# define _GCM_GEOLIB_FILTER_PLACEDBY_H

#include <geolib/Filter.h>
#include <stl/String.h>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API PlacedBy: public Filter {
	public:
		static const int ID;
		virtual int getId() const;
		virtual String getName() const;
		virtual bool matchCache(GC<Geocache> cache);
		virtual bool matchWaypoint(GC<GeocacheWaypoint> waypoint);
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> waypoint);
		virtual void toXml(_xmlNode *root);
		virtual bool fromXml(_xmlNode *root);

		void setPlacedBy(String placedBy);
		String getPlacedBy();

	private:
		String placedBy;
};

		}
	}
}

#endif
