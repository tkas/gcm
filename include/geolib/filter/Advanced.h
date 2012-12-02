#ifndef _GCM_GEOLIB_FILTER_ADVANCED_H
# define _GCM_GEOLIB_FILTER_ADVANCED_H

#include <geolib/Filter.h>
#include <list>

namespace GCM {
	namespace geolib {
		namespace filter {

class GCM_API AdvancedFilterItem {
	public:
		typedef enum {
			Initial,
			Add,
			Subtract,
			Intersect
		} Mode;

		AdvancedFilterItem(): mode(Initial), filter(), name() {}

		AdvancedFilterItem(String name, Mode mode, GC<Filter> filter):
			mode(mode), filter(filter), name(name) {}

		Mode getMode() { return this->mode; }
		GC<Filter> getFilter() { return this->filter; }
		String getName() { return this->name; }
		bool operator==(const AdvancedFilterItem &item) const { return name->equals(item.name) && mode == item.mode && filter == item.filter; }
		bool operator<(const AdvancedFilterItem &item) const { return name < item.name; }

	private:
		Mode mode;
		GC<Filter> filter;
		String name;
};

class GCM_API Advanced: public Filter, public std::list<AdvancedFilterItem> {
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
