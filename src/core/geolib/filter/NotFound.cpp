#include <geolib/filter/NotFound.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API NotFound::getId() const {
	return NotFound::ID;
}

String GCM_API NotFound::getName() const {
	return "NotFound";
}

bool GCM_API NotFound::matchCache(GC<Geocache> cache) {
	return !cache->isFound();
}

bool GCM_API NotFound::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API NotFound::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
