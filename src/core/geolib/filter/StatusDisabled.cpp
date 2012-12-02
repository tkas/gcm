#include <geolib/filter/StatusDisabled.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API StatusDisabled::getId() const {
	return StatusDisabled::ID;
}

String GCM_API StatusDisabled::getName() const {
	return "StatusDisabled";
}

bool GCM_API StatusDisabled::matchCache(GC<Geocache> cache) {
	return !cache->isDisabled();
}

bool GCM_API StatusDisabled::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API StatusDisabled::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
