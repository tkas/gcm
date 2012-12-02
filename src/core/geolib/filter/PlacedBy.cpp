#include <geolib/filter/PlacedBy.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API PlacedBy::getId() const {
	return PlacedBy::ID;
}

String GCM_API PlacedBy::getName() const {
	return "PlacedBy";
}

void GCM_API PlacedBy::setPlacedBy(String placedBy) {
	this->placedBy = placedBy->toLowerCase();
}

String GCM_API PlacedBy::getPlacedBy() {
	return this->placedBy;
}

bool GCM_API PlacedBy::matchCache(GC<Geocache> cache) {
	return cache->getPlacedBy()->toLowerCase()->equals(this->placedBy);
}

bool GCM_API PlacedBy::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API PlacedBy::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

void GCM_API PlacedBy::toXml(xmlNodePtr root) {
	xmlAddChild(root, xmlNewText((const xmlChar *)this->placedBy->c_str()));
}

bool GCM_API PlacedBy::fromXml(xmlNodePtr root) {
	xmlChar *cContent = xmlNodeListGetString(root->doc, root->children, 1);
	if (cContent) {
		this->setPlacedBy((const char *)cContent);
		xmlFree(cContent);

		return this->getPlacedBy()->length() > 0;
	} else {
		return false;
	}
}
