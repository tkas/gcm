#include <geolib/filter/Terrain.h>
#include <stl/String.h>
#include <string.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API Terrain::getId() const {
	return Terrain::ID;
}

String GCM_API Terrain::getName() const {
	return "Terrain";
}

void GCM_API Terrain::include(int value) {
	this->list[value] = true;
}

void GCM_API Terrain::exclude(int value) {
	this->list[value] = false;
}

bool GCM_API Terrain::contains(int value) {
	std::map<int, bool>::iterator i = this->list.find(value);
	if (i == this->list.end()) return false;
	return i->second;
}

void GCM_API Terrain::clear() {
	this->list.clear();
}

bool GCM_API Terrain::matchCache(GC<Geocache> cache) {
	std::map<int, bool>::iterator i = this->list.find(cache->getTerrain());
	return i != this->list.end() && i->second;
}

bool GCM_API Terrain::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

bool GCM_API Terrain::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

void GCM_API Terrain::toXml(xmlNodePtr root) {
	std::map<int, bool>::iterator i = this->list.begin();
	while (i != this->list.end()) {
		if (i->second) {
			double value = i->first / 10;
			xmlNewChild(root, NULL, (const xmlChar *)"value", (const xmlChar *)String::format("%1.*lf", i->first % 2 == 1, value)->c_str());
		}

		i++;
	}
}

bool GCM_API Terrain::fromXml(xmlNodePtr root) {
	xmlNodePtr child = root->children;
	while (child != NULL) {
		if (child->type == XML_ELEMENT_NODE && strcmp((const char *)child->name, "value") == 0) {
			xmlChar *cContent = xmlNodeListGetString(child->doc, child->children, 1);
			if (cContent) {
				int value = strtod((const char *)cContent, NULL) * 10;
				this->include(value);
				xmlFree(cContent);
			}
		}

		child = child->next;
	}

	return this->list.size() > 0;
}
