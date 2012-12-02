#include "ListFilter.h"

void ListFilter::append(GCM::String gcid) {
	this->filteredIDs[gcid] = true;
}

bool ListFilter::matchCache(GCM::GC<GCM::geolib::Geocache> cache) {
	std::map<GCM::String, bool>::iterator i = filteredIDs.find(cache->getId());
	if (i != filteredIDs.end()) {
		return true;
	} else {
		return false;
	}
}

bool ListFilter::matchWaypoint(GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool ListFilter::matchGenericWaypoint(GCM::GC<GCM::geolib::GenericWaypoint> waypoint) {
	return false;
	(void)waypoint;
}

int ListFilter::getId() const {
	return Filter::USER + 1;
}

GCM::String ListFilter::getName() const {
	return "ListFilter";
}
