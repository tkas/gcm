#ifndef _GCM_GEOLIB_FILTER_CACHENAME_H
# define _GCM_GEOLIB_FILTER_CACHENAME_H

#include <geolib/Filter.h>
#include <stl/String.h>
#include <list>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API CacheName: public Filter {
	public:
		typedef enum {
			MM_IGNORED,
			MM_CONTAINS,
			MM_DN_CONTAINS,
			MM_BEGINS,
			MM_ENDS,
			MM_EQUALS
		} MatchMode;

		CacheName();

		static const int ID;
		virtual int getId() const;
		virtual String getName() const;
		virtual bool matchCache(GC<Geocache> cache);
		virtual bool matchWaypoint(GC<GeocacheWaypoint> waypoint);
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> waypoint);
		virtual void toXml(_xmlNode *root);
		virtual bool fromXml(_xmlNode *root);

		void setCacheName(String name);
		String getCacheName();
		void setMatchMode(MatchMode mm);
		MatchMode getMatchMode();

	private:
		String name;
		std::list<String> nameParts;
		MatchMode matchMode;

		void updateParts();
		bool contains(String cacheName);
};

		}
	}
}

#endif
