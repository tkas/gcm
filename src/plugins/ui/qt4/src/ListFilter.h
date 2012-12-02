#ifndef LISTFILTER_H
#define LISTFILTER_H

#include <geolib/Filter.h>
#include <map>
#include <stl/String.h>

class ListFilter: public GCM::geolib::Filter {
private:
	std::map<GCM::String, bool> filteredIDs;

public:
	void append(GCM::String gcid);
	bool matchCache(GCM::GC<GCM::geolib::Geocache> cache);
	bool matchWaypoint(GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint);
	bool matchGenericWaypoint(GCM::GC<GCM::geolib::GenericWaypoint> waypoint);
	int getId() const;
	GCM::String getName() const;
};

#endif // LISTFILTER_H
