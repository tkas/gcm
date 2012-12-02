#include <geolib/filter/StatusArchived.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API StatusArchived::getId() const {
	return StatusArchived::ID;
}

String GCM_API StatusArchived::getName() const {
	return "StatusArchived";
}

bool GCM_API StatusArchived::matchCache(GC<Geocache> cache) {
	return !cache->isArchived();
}

bool GCM_API StatusArchived::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API StatusArchived::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
