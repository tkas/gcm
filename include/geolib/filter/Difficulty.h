#ifndef _GCM_GEOLIB_FILTER_DIFFICULTY_H
# define _GCM_GEOLIB_FILTER_DIFFICULTY_H

#include <geolib/Filter.h>
#include <stl/String.h>
#include <map>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API Difficulty: public Filter {
	public:
		static const int ID;
		virtual int getId() const;
		virtual String getName() const;
		virtual bool matchCache(GC<Geocache> cache);
		virtual bool matchWaypoint(GC<GeocacheWaypoint> waypoint);
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> waypoint);
		void include(int value);
		void exclude(int value);
		bool contains(int value);
		void clear();
		virtual void toXml(_xmlNode *root);
		virtual bool fromXml(_xmlNode *root);

	private:
		std::map<int, bool> list;
};

		}
	}
}

#endif
