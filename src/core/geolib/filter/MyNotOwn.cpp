#include <geolib/filter/MyNotOwn.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API MyNotOwn::getId() const {
	return MyNotOwn::ID;
}

String GCM_API MyNotOwn::getName() const {
	return "MyNotOwn";
}

bool GCM_API MyNotOwn::matchCache(GC<Geocache> cache) {
	return !cache->isOwn();
}

bool GCM_API MyNotOwn::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API MyNotOwn::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
