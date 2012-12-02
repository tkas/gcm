#include <geolib/filter/DoesNotHaveCorrectedOrFinal.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API DoesNotHaveCorrectedOrFinal::getId() const {
	return DoesNotHaveCorrectedOrFinal::ID;
}

String GCM_API DoesNotHaveCorrectedOrFinal::getName() const {
	return "DoesNotHaveCorrectedOrFinal";
}

bool GCM_API DoesNotHaveCorrectedOrFinal::matchCache(GC<Geocache> cache) {
	return !cache->hasCorrectedCoords();
}

bool GCM_API DoesNotHaveCorrectedOrFinal::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API DoesNotHaveCorrectedOrFinal::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
