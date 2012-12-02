#include <geolib/filter/Distance.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

Distance::Distance(): Filter(), mUseCacheDistance(true) {
}

int GCM_API Distance::getId() const {
	return Distance::ID;
}

String GCM_API Distance::getName() const {
	return "Distance";
}

void GCM_API Distance::setDistance(int distance) {
	this->distance = distance;
}

int GCM_API Distance::getDistance() {
	return this->distance;
}

void Distance::setOrigin(int latE6, int lonE6) {
	this->latitudeE6 = latE6;
	this->longitudeE6 = lonE6;
}

int Distance::getLatitudeE6() {
	return this->latitudeE6;
}

int Distance::getLongitudeE6() {
	return this->longitudeE6;
}

void Distance::setUseCacheDistance(bool useCacheDistance) {
	this->mUseCacheDistance = useCacheDistance;
}

bool Distance::useCacheDistance() {
	return this->mUseCacheDistance;
}

bool GCM_API Distance::matchCache(GC<Geocache> cache) {
	if (this->mUseCacheDistance) {
		return cache->getDistance().distance < this->distance;
	} else {
		return cache->calcDistance(this->latitudeE6, this->longitudeE6) < this->distance;
	}
}

bool GCM_API Distance::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API Distance::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

void GCM_API Distance::toXml(xmlNodePtr root) {
	char buffer[128];
	
	if (!this->mUseCacheDistance) {
		snprintf(buffer, 128, "%lf", this->getLatitudeE6() / 1E6);
		xmlSetProp(root, (const xmlChar *)"lat", (const xmlChar *)buffer);
	
		snprintf(buffer, 128, "%lf", this->getLongitudeE6() / 1E6);
		xmlSetProp(root, (const xmlChar *)"lon", (const xmlChar *)buffer);
	}
	
	snprintf(buffer, 128, "%d", this->getDistance());
	xmlSetProp(root, (const xmlChar *)"distance", (const xmlChar *)buffer);
}

bool GCM_API Distance::fromXml(xmlNodePtr root) {
	const char *prop;
	
	if ((prop = (const char *)xmlGetProp(root, (const xmlChar *)"distance")) != NULL) {
		this->setDistance(atoi(prop));
		this->setUseCacheDistance(true);
	} else {
		return false;
	}
	
	if ((prop = (const char *)xmlGetProp(root, (const xmlChar *)"lat")) != NULL) {
		int lat = atof(prop) * 1E6;
		if ((prop = (const char *)xmlGetProp(root, (const xmlChar *)"lon")) != NULL) {
			int lon = atof(prop) * 1E6;
			this->setOrigin(lat, lon);
			this->setUseCacheDistance(false);
		}
	}
	
	return true;
}
