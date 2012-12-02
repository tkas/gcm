#include <geolib/filter/HasCorrectedOrFinal.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API HasCorrectedOrFinal::getId() const {
	return HasCorrectedOrFinal::ID;
}

String GCM_API HasCorrectedOrFinal::getName() const {
	return "HasCorrectedOrFinal";
}

bool GCM_API HasCorrectedOrFinal::matchCache(GC<Geocache> cache) {
	return cache->hasCorrectedCoords();
}

bool GCM_API HasCorrectedOrFinal::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API HasCorrectedOrFinal::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
