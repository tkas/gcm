#ifndef _GCM_GEOLIB_FILTER_DISTANCE_H
# define _GCM_GEOLIB_FILTER_DISTANCE_H

#include <geolib/Filter.h>
#include <stl/String.h>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API Distance: public Filter {
	public:
		Distance();
	
		static const int ID;
		virtual int getId() const;
		virtual String getName() const;
		
		virtual void setDistance(int distance);
		virtual int getDistance();
		
		virtual void setOrigin(int latE6, int lonE6);
		virtual int getLatitudeE6();
		virtual int getLongitudeE6();
		
		virtual void setUseCacheDistance(bool useCacheDistance);
		virtual bool useCacheDistance();
		
		virtual bool matchCache(GC<Geocache> cache);
		virtual bool matchWaypoint(GC<GeocacheWaypoint> waypoint);
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> waypoint);
		
		virtual void toXml(_xmlNode *root);
		virtual bool fromXml(_xmlNode *root);
		
	private:
		int distance;
		int latitudeE6;
		int longitudeE6;
		bool mUseCacheDistance;
};

		}
	}
}

#endif
