#include <geolib/filter/Found.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API Found::getId() const {
	return Found::ID;
}

String GCM_API Found::getName() const {
	return "Found";
}

bool GCM_API Found::matchCache(GC<Geocache> cache) {
	return cache->isFound();
}

bool GCM_API Found::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API Found::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
