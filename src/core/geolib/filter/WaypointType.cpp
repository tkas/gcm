#include <geolib/filter/WaypointType.h>
#include <stl/String.h>
#include <string.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API WaypointType::getId() const {
	return WaypointType::ID;
}

String GCM_API WaypointType::getName() const {
	return "WaypointType";
}

void GCM_API WaypointType::include(GeocacheWaypoint::Type value) {
	this->list[value] = true;
}

void GCM_API WaypointType::exclude(GeocacheWaypoint::Type value) {
	this->list[value] = false;
}

bool GCM_API WaypointType::contains(GeocacheWaypoint::Type value) {
	std::map<GeocacheWaypoint::Type, bool>::iterator i = this->list.find(value);
	if (i == this->list.end()) return false;
	return i->second;
}

void GCM_API WaypointType::clear() {
	this->list.clear();
}

bool GCM_API WaypointType::matchCache(GC<Geocache> cache) {
	return true;
	(void)cache;
	}

bool GCM_API WaypointType::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	std::map<GeocacheWaypoint::Type, bool>::iterator i = this->list.find(waypoint->getWptType());
	return i != this->list.end() && i->second;

}

bool GCM_API WaypointType::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

void GCM_API WaypointType::toXml(xmlNodePtr root) {
	std::map<GeocacheWaypoint::Type, bool>::iterator i = this->list.begin();
	while (i != this->list.end()) {
		if (i->second) {
			xmlNewChild(root, NULL, (const xmlChar *)"value", (const xmlChar *)GeocacheWaypoint::typeToGpxString(i->first)->c_str());
		}

		i++;
	}
}

bool GCM_API WaypointType::fromXml(xmlNodePtr root) {
	xmlNodePtr child = root->children;
	while (child != NULL) {
		if (child->type == XML_ELEMENT_NODE && strcmp((const char *)child->name, "value") == 0) {
			xmlChar *cContent = xmlNodeListGetString(child->doc, child->children, 1);
			if (cContent) {
				this->include(GeocacheWaypoint::gpxStringToType((const char *)cContent));
				xmlFree(cContent);
			}
		}

		child = child->next;
	}

	return this->list.size() > 0;
}
