#include <geolib/filter/MatchAll.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API MatchAll::getId() const {
	return MatchAll::ID;
}

String GCM_API MatchAll::getName() const {
	return "MatchAll";
}

bool GCM_API MatchAll::matchCache(GC<Geocache> cache) {
	return true;
	(void)cache;
}

bool GCM_API MatchAll::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API MatchAll::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
