#include <geolib/filter/StatusAvailable.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API StatusAvailable::getId() const {
	return StatusAvailable::ID;
}

String GCM_API StatusAvailable::getName() const {
	return "StatusAvailable";
}

bool GCM_API StatusAvailable::matchCache(GC<Geocache> cache) {
	return !cache->isAvailable();
}

bool GCM_API StatusAvailable::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API StatusAvailable::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
