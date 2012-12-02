#include <geolib/filter/Size.h>
#include <stl/String.h>
#include <string.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API Size::getId() const {
	return Size::ID;
}

String GCM_API Size::getName() const {
	return "Size";
}

void GCM_API Size::include(Geocache::Size value) {
	this->list[value] = true;
}

void GCM_API Size::exclude(Geocache::Size value) {
	this->list[value] = false;
}

bool GCM_API Size::contains(Geocache::Size value) {
	std::map<Geocache::Size, bool>::iterator i = this->list.find(value);
	if (i == this->list.end()) return false;
	return i->second;
}

void GCM_API Size::clear() {
	this->list.clear();
}

bool GCM_API Size::matchCache(GC<Geocache> cache) {
	std::map<Geocache::Size, bool>::iterator i = this->list.find(cache->getSize());
	return i != this->list.end() && i->second;
}

bool GCM_API Size::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API Size::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

void GCM_API Size::toXml(xmlNodePtr root) {
	std::map<Geocache::Size, bool>::iterator i = this->list.begin();
	while (i != this->list.end()) {
		if (i->second) {
			xmlNewChild(root, NULL, (const xmlChar *)"value", (const xmlChar *)Geocache::sizeToGpxString(i->first)->c_str());
		}

		i++;
	}
}

bool Size::fromXml(xmlNodePtr root) {
	xmlNodePtr child = root->children;
	while (child != NULL) {
		if (child->type == XML_ELEMENT_NODE && strcmp((const char *)child->name, "value") == 0) {
			xmlChar *cContent = xmlNodeListGetString(child->doc, child->children, 1);
			if (cContent) {
				this->include(Geocache::gpxStringToSize((const char *)cContent));
				xmlFree(cContent);
			}
		}

		child = child->next;
	}

	return this->list.size() > 0;
}
