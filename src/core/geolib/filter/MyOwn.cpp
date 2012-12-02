#include <geolib/filter/MyOwn.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API MyOwn::getId() const {
	return MyOwn::ID;
}

String GCM_API MyOwn::getName() const {
	return "MyOwn";
}

bool GCM_API MyOwn::matchCache(GC<Geocache> cache) {
	return cache->isOwn();
}

bool GCM_API MyOwn::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API MyOwn::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
