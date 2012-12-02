#include <geolib/filter/GeocacheType.h>
#include <stl/String.h>
#include <string.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API GeocacheType::getId() const {
	return GeocacheType::ID;
}

String GCM_API GeocacheType::getName() const {
	return "GeocacheType";
}

void GCM_API GeocacheType::include(Geocache::Type value) {
	this->list[value] = true;
}

void GCM_API GeocacheType::exclude(Geocache::Type value) {
	this->list[value] = false;
}

bool GCM_API GeocacheType::contains(Geocache::Type value) {
	std::map<Geocache::Type, bool>::iterator i = this->list.find(value);
	if (i == this->list.end()) return false;
	return i->second;
}

void GCM_API GeocacheType::clear() {
	this->list.clear();
}

bool GCM_API GeocacheType::matchCache(GC<Geocache> cache) {
	std::map<Geocache::Type, bool>::iterator i = this->list.find(cache->getCacheType());
	return i != this->list.end() && i->second;
}

bool GCM_API GeocacheType::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API GeocacheType::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}
void GCM_API GeocacheType::toXml(xmlNodePtr root) {
	std::map<Geocache::Type, bool>::iterator i = this->list.begin();
	while (i != this->list.end()) {
		if (i->second) {
			xmlNewChild(root, NULL, (const xmlChar *)"value", (const xmlChar *)Geocache::typeToGpxString(i->first)->c_str());
		}

		i++;
	}
}

bool GeocacheType::fromXml(xmlNodePtr root) {
	xmlNodePtr child = root->children;
	while (child != NULL) {
		if (child->type == XML_ELEMENT_NODE && strcmp((const char *)child->name, "value") == 0) {
			xmlChar *cContent = xmlNodeListGetString(child->doc, child->children, 1);
			if (cContent) {
				this->include(Geocache::gpxStringToType((const char *)cContent));
				xmlFree(cContent);
			}
		}

		child = child->next;
	}

	return this->list.size() > 0;
}
